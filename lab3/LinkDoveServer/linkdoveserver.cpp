#include "linkdoveserver.h"

#include <thread>
#include <mutex>
#include <iostream>
#include <iterator>
#include <boost/asio/ip/tcp.hpp>

#include <QSqlError>
#include <QDebug>

#include "UserInfo.h"
#include "channelinfo.h"
#include "utility.h"

std::mutex connection_mutex;

#define DATABASE_CONNECTION_NAME "LinkDoveConnection"

LinkDoveServer::LinkDoveServer()
    : io_context_ptr_(std::make_shared<asio::io_context>())
    , acceptor_(*io_context_ptr_)
    , data_base_(DATABASE_CONNECTION_NAME)
{
    setup_connection_tree();
}

LinkDoveServer::~LinkDoveServer() {
    acceptor_.close();
}

void LinkDoveServer::listen(uint16_t port) {
    auto endpoint = tcp::endpoint(tcp::v4(), port);

    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(tcp::acceptor::reuse_address(true));
    acceptor_.bind(endpoint);
    acceptor_.listen();

    start_async_accept();
}

void LinkDoveServer::setup_connection_tree() {
    handle_tree_[LOGIN_REQUEST]                      = &LinkDoveServer::handle_login_request;
    handle_tree_[REGISTER_REQUEST]                   = &LinkDoveServer::handle_register_request;
    handle_tree_[SEND_COMPLAINT_REQUEST]             = &LinkDoveServer::handle_send_complaint_request;
    handle_tree_[DEL_COMPLAINT_REQUEST]              = &LinkDoveServer::handle_del_complaint_request;
    handle_tree_[GET_COMPLAINTS_REQUEST]             = &LinkDoveServer::handle_get_complaints_request;
    handle_tree_[UPDATE_USER_REQUEST]                = &LinkDoveServer::handle_update_user_request;
    handle_tree_[FIND_USER_REQUEST]                  = &LinkDoveServer::handle_find_user_request;
    handle_tree_[BAN_USER_REQUEST]                   = &LinkDoveServer::handle_ban_user_request;
    handle_tree_[SEND_MSG_REQUEST]                   = &LinkDoveServer::handle_send_msg_request;
    handle_tree_[GET_IND_MSG_REQUEST]                = &LinkDoveServer::handle_get_msg_request;
    handle_tree_[GET_INTERLOCUTORS_REQUEST]          = &LinkDoveServer::handle_get_interlocutors_request;
    handle_tree_[CREATE_CHANNEL_REQUEST]             = &LinkDoveServer::handle_channel_create_request;
    handle_tree_[FIND_CHANNEL_REQUEST]               = &LinkDoveServer::handle_find_channel_request;
    handle_tree_[GET_CHANNELS_REQUEST]               = &LinkDoveServer::handle_get_channels_request;
    handle_tree_[IS_CHANNEL_PARTICIPANT_REQUEST]     = &LinkDoveServer::handle_is_channel_participant_request;
    handle_tree_[ADD_PARTICIPANT_TO_CHANNEL_REQUEST] = &LinkDoveServer::handle_add_channel_participant_request;
    handle_tree_[GET_CHNNL_MSG_REQUEST]              = &LinkDoveServer::handle_get_channel_messages_request;
    handle_tree_[DELETE_CHANNEL_REQUEST]             = &LinkDoveServer::handle_delete_channel;
    handle_tree_[CREATE_CHAT_REQUEST]                = &LinkDoveServer::handle_create_chat_request;
    handle_tree_[IS_CHAT_PARTICIPANT_REQUEST]        = &LinkDoveServer::handle_is_chat_participant_request;
    handle_tree_[ADD_PARTICIPANT_TO_CHAT_REQUEST]    = &LinkDoveServer::handle_add_chat_participant_request;
    handle_tree_[GET_CHATS_REQUEST]                  = &LinkDoveServer::handle_get_chats_request;
    handle_tree_[FIND_CHAT_REQUEST]                  = &LinkDoveServer::handle_find_chat_request;
    handle_tree_[GET_CHAT_MSG_REQUEST]               = &LinkDoveServer::handle_get_chat_messages_request;
    handle_tree_[DELETE_CHAT_REQUEST]                = &LinkDoveServer::handle_delete_chat;
    handle_tree_[DELETE_IND_CHAT_REQUEST]            = &LinkDoveServer::handle_delete_ind_chat;
    handle_tree_[QUIT_CHAT_REQUEST]                  = &LinkDoveServer::handle_quit_chat;
    handle_tree_[QUIT_CHANNEL_REQUEST]               = &LinkDoveServer::handle_quit_channel;
    handle_tree_[REMOVE_USER_FROM_CHANNEL_REQUEST]   = &LinkDoveServer::handle_remove_user_from_channel;
    handle_tree_[REMOVE_USER_FROM_CHAT_REQUEST]      = &LinkDoveServer::handle_remove_user_from_chat;
    handle_tree_[GET_CHNNL_PARTICIPANTS_REQUEST]     = &LinkDoveServer::handle_get_channel_participants_request;
    handle_tree_[GET_CHAT_PARTICIPANTS_REQUEST]      = &LinkDoveServer::handle_get_chat_participants_request;
    handle_tree_[SEND_USER_ANSWER_REQUEST]           = &LinkDoveServer::handle_answer_user;
    handle_tree_[GET_NOTIFICATIONS_REQUEST]          = &LinkDoveServer::handle_get_notifications;
    handle_tree_[DEL_NOTIFICATION_REQUEST]           = &LinkDoveServer::handle_del_notification;
    handle_tree_[DEL_MSG_REQUEST]                    = &LinkDoveServer::handle_delete_msg;

    for (auto iter = handle_tree_.begin(); iter != handle_tree_.end(); ++iter) {
        std::cerr << iter->first << '\n';
    }

    try {
        handle_tree_.at(LOGIN_REQUEST);
    } catch (...) {
        std::cerr << "NO METHOD\n";
    }
}

void LinkDoveServer::start_async_accept() {
    connection_mutex.lock();
    connections_.emplace_back(io_context_ptr_);
    ConnectionIterator iterator = --connections_.end();
    connection_mutex.unlock();

    acceptor_.async_accept(iterator->socket_,
                            boost::bind(&LinkDoveServer::handle_async_accept,
                                        shared_from_this(),
                                        iterator,
                                        boost::asio::placeholders::error));
    run_context();
}

void LinkDoveServer::async_read(ConnectionIterator iterator) {
    asio::async_read_until(iterator->socket_,
                           iterator->buffer_,
                           END_OF_REQUEST,
                           boost::bind(&LinkDoveServer::handle_async_read,
                                       shared_from_this(),
                                       iterator,
                                       asio::placeholders::error,
                                       asio::placeholders::bytes_transferred));
    run_context();
}

void LinkDoveServer::async_write(ConnectionIterator iterator) {
    asio::async_write(iterator->socket_, iterator->buffer_,
                      boost::bind(&LinkDoveServer::handle_async_write,
                                  shared_from_this(),
                                  iterator,
                                  asio::placeholders::error,
                                  asio::placeholders::bytes_transferred));

    run_context();
}

void LinkDoveServer::async_close_write(ConnectionIterator iterator) {
    asio::async_write(iterator->socket_, iterator->buffer_,
                      boost::bind(&LinkDoveServer::handle_async_close_write,
                                  shared_from_this(),
                                  iterator,
                                  asio::placeholders::error,
                                  asio::placeholders::bytes_transferred));

    iterator->socket_.close();
    run_context();
}

void LinkDoveServer::handle_async_accept(ConnectionIterator iterator, boost::system::error_code error) {
    if (error) {
        std::cerr << "Failed to accept new connection: " << error.message() << '\n';
        async_close_write(iterator);
    } else {
        std::cout << "Received connection from: " << iterator->socket_.remote_endpoint().address() << "\n";
    }

    async_read(iterator);
    start_async_accept();
}

void LinkDoveServer::handle_async_read(ConnectionIterator iterator, boost::system::error_code error, size_t bytes_transfered) {
    if (error) {
        std::cerr << "Failed to read from socket: " << error.value() << ' ' << error.message() << ". Closing it.\n";
        async_close_write(iterator);
    }

    if (bytes_transfered > 0) {
        handle_type_request(iterator);
    }
}

void LinkDoveServer::handle_async_write(ConnectionIterator iterator, boost::system::error_code error, size_t bytes_transfered) {
    if (error) {
        std::cerr << "Failed to write to socket: " << error.value() << ' ' << error.message() << ". Closing it.";
        async_close_write(iterator);
    }

    async_read(iterator);
}

void LinkDoveServer::handle_async_close_write(ConnectionIterator iterator, boost::system::error_code error, size_t bytes_transfered) {
    if (error) {
        std::cerr << "Failed to write close requests to socket: " << error.value() << ' ' << error.message();
    }

    connections_.erase(iterator);
}

void LinkDoveServer::handle_type_request(ConnectionIterator iterator) {
    std::string request_type;
    std::getline(iterator->in_stream_, request_type);

    (this->*handle_tree_[request_type.c_str()])(iterator);
}

void LinkDoveServer::handle_login_request(ConnectionIterator iterator) {
    LoginInfo login_info;
    login_info.deserialize(iterator->in_stream_);

    remove_delimeter(iterator);

    if (data_base_.login_user(login_info)) {
        try {
            iterator->out_stream_ << LOGIN_SUCCESS << "\n";

            StatusInfo status_info = data_base_.get_status_info(login_info.username_);
            status_info.serialize(iterator->out_stream_);

        } catch (std::runtime_error &error) {
            std::cerr << "Failed to login user: " << error.what() << '\n';
            iterator->out_stream_ << LOGIN_FAILED << "\n";
        }
    } else {
        iterator->out_stream_ << LOGIN_FAILED << "\n";
    }

    iterator->out_stream_ << END_OF_REQUEST;
    async_write(iterator);
}

void LinkDoveServer::handle_register_request(ConnectionIterator iterator) {
    UserInfo user_info;
    user_info.deserialize(iterator->in_stream_);

    remove_delimeter(iterator);

    std::stringstream answer;
    if (data_base_.register_user(user_info)) {
        // В случае удачной регистрации отправляем клиенту структуру StatusInfo (в ней содержится ID)
        user_info.status_info_ = data_base_.get_status_info(user_info.status_info_.username_);

        answer << REGISTER_SUCCESS << "\n";
        user_info.status_info_.serialize(answer);
        answer << END_OF_REQUEST;
    } else {
        answer << REGISTER_FAILED << "\n" << END_OF_REQUEST;
    }

    iterator->out_stream_ << answer.str();
    async_write(iterator);
}

void LinkDoveServer::handle_send_complaint_request(ConnectionIterator iterator) {
    Complaint complaint;
    complaint.deserialize(iterator->in_stream_);

    remove_delimeter(iterator);

    std::stringstream answer;
    if (data_base_.add_complaint(complaint)) {
        answer << SEND_COMPLAINT_SUCCESS << "\n" << END_OF_REQUEST;
    } else {
        answer << SEND_COMPLAINT_FAILED << "\n" << END_OF_REQUEST;
    }

    iterator->out_stream_ << answer.str();
    async_write(iterator);
}

void LinkDoveServer::handle_del_complaint_request(ConnectionIterator iterator) {
    unsigned long long complaint_id = UtilitySerializator::deserialize_fundamental<unsigned long long>(iterator->in_stream_).second;

    remove_delimeter(iterator);

    std::stringstream answer;
    if (data_base_.del_complaint(complaint_id)) {
        answer << DEL_COMPLAINT_SUCCESS << "\n" << END_OF_REQUEST;
    } else {
        answer << DEL_COMPLAINT_FAILED << "\n" << END_OF_REQUEST;
    }

    iterator->out_stream_ << answer.str();
    async_write(iterator);
}

void LinkDoveServer::handle_get_complaints_request(ConnectionIterator iterator) {
    remove_delimeter(iterator);

    size_t complaints_count = 0;
    std::stringstream answer;
    if ((complaints_count = data_base_.get_complaints_count()) < 0) {
        answer << GET_COMPLAINTS_FAILED << "\n" << END_OF_REQUEST;
    } else {
        complaints_count = complaints_count > GET_COMPLAINTS_LIMIT ? GET_COMPLAINTS_LIMIT : complaints_count;

        std::vector<Complaint> complaints;
        try {
            complaints = data_base_.get_complaints(complaints_count);

            answer << GET_COMPLAINTS_SUCCESS << "\n";
            UtilitySerializator::serialize(answer, complaints);
            answer << END_OF_REQUEST;
        } catch (std::runtime_error& ex) {
            std::cerr << "Failed to get complaints: " << ex.what() << '\n';
            answer << GET_COMPLAINTS_FAILED << "\n" << END_OF_REQUEST;
        }

        iterator->out_stream_ << answer.str();
        async_write(iterator);
    }
}

void LinkDoveServer::handle_update_user_request(ConnectionIterator iterator) {
    StatusInfo status_info;
    status_info.deserialize(iterator->in_stream_);

    remove_delimeter(iterator);

    std::stringstream answer;
    if (data_base_.update_user(status_info)) {
        answer << UPDATE_USER_SUCCESS << "\n" << END_OF_REQUEST;
    } else {
        answer << UPDATE_USER_FAILED << "\n" << END_OF_REQUEST;
    }

    iterator->out_stream_ << answer.str();
    async_write(iterator);
}

void LinkDoveServer::handle_find_user_request(ConnectionIterator iterator) {
    std::string username = UtilitySerializator::deserialize_string(iterator->in_stream_).second;

    remove_delimeter(iterator);

    std::stringstream answer;
    try {
        StatusInfo status_info;
        status_info = data_base_.get_status_info(username);

        answer << FIND_USER_SUCCESS << "\n";
        status_info.serialize(answer);
        answer << END_OF_REQUEST;
    } catch(std::runtime_error& ex) {
        std::cerr << "Failed to find user: " << ex.what() << '\n';
        answer << FIND_USER_FAILED << "\n" << END_OF_REQUEST;
    }

    iterator->out_stream_ << answer.str();
    async_write(iterator);
}

void LinkDoveServer::handle_ban_user_request(ConnectionIterator iterator) {
    std::string username = UtilitySerializator::deserialize_string(iterator->in_stream_).second;
    uint8_t is_ban = UtilitySerializator::deserialize_fundamental<uint8_t>(iterator->in_stream_).second;

    remove_delimeter(iterator);

    std::stringstream answer;
    if (data_base_.ban_user(username, is_ban)) {
        answer << BAN_USER_SUCCESS << "\n" << END_OF_REQUEST;
    } else {
        answer << BAN_USER_FAILED << "\n" << END_OF_REQUEST;
    }

    iterator->out_stream_ << answer.str();
    async_write(iterator);
}

void LinkDoveServer::handle_send_msg_request(ConnectionIterator iterator) {
    std::shared_ptr<IMessage> msg_ptr = UtilitySerializator::deserialize_msg(iterator->in_stream_).second;

    remove_delimeter(iterator);

    std::stringstream answer;
    switch (msg_ptr->get_msg_type()) {
        case INDIVIDUAL_MSG_TYPE: {
            if (data_base_.add_ind_message(*msg_ptr)) {
                answer << SEND_IND_MSG_SUCCESS << "\n" << END_OF_REQUEST;
            } else {
                answer << SEND_IND_MSG_FAILED << "\n" << END_OF_REQUEST;
            }

            break;
        }

        case CHANNEL_MSG_TYPE: {
            if (data_base_.add_chnnl_message(*msg_ptr)) {
                answer << SEND_CHNNL_MSG_SUCCESS << "\n" << END_OF_REQUEST;
            } else {
                answer << SEND_CHNNL_MSG_FAILED << "\n" << END_OF_REQUEST;
            }

            break;
        }

        case GROUP_MSG_TYPE: {
            if (data_base_.add_chat_message(*msg_ptr)) {
                answer << SEND_CHAT_MSG_SUCCESS << "\n" << END_OF_REQUEST;
            } else {
                answer << SEND_CHAT_MSG_FAILED << "\n" << END_OF_REQUEST;
            }

            break;
        }
    }

    iterator->out_stream_ << answer.str();
    async_write(iterator);
}

void LinkDoveServer::handle_get_msg_request(ConnectionIterator iterator) {
    unsigned long long first_id = 0, second_id = 0; // идентификаторы двух собеседников чата

    first_id = UtilitySerializator::deserialize_fundamental<unsigned long long>(iterator->in_stream_).second;
    second_id = UtilitySerializator::deserialize_fundamental<unsigned long long>(iterator->in_stream_).second;

    remove_delimeter(iterator);

    std::stringstream answer;
    try { // получаем сообщения от first_id к second_id и наоборот
        std::vector<std::shared_ptr<IMessage>> first_messages = data_base_.get_ind_messages(first_id, second_id);
        std::vector<std::shared_ptr<IMessage>> second_messages = data_base_.get_ind_messages(second_id, first_id);

        std::vector<std::shared_ptr<IMessage>> all_messages;
        all_messages.reserve(first_messages.size() + second_messages.size());

        all_messages.insert(all_messages.end(), first_messages.begin(),  first_messages.end());
        all_messages.insert(all_messages.end(), second_messages.begin(),  second_messages.end());

        // сортируем сообщения по дате
        std::sort(all_messages.begin(), all_messages.end(), message_ptr_less_comparator());

        answer << GET_IND_MSG_SUCCESS << "\n";
        UtilitySerializator::serialize(answer, all_messages);
        answer << END_OF_REQUEST;

    } catch (std::runtime_error& ex) {
        std::cerr << "Failed to get messages: " << ex.what() << '\n';
        answer << GET_IND_MSG_FAILED << "\n" << END_OF_REQUEST;
    }

    iterator->out_stream_ << answer.str();
    async_write(iterator);
}

void LinkDoveServer::handle_get_interlocutors_request(ConnectionIterator iterator) {
    unsigned long long id = UtilitySerializator::deserialize_fundamental<unsigned long long>(iterator->in_stream_).second;

    remove_delimeter(iterator);

    std::stringstream answer;
    try {
        std::vector<StatusInfo> interlocutors = data_base_.get_interlocutors(id);

        answer << GET_INTERLOCUTORS_SUCCESS << "\n";
        UtilitySerializator::serialize(answer, interlocutors);
        answer << END_OF_REQUEST;
    } catch (std::runtime_error &ex) {
        std::cerr << "Failed to get interlocutors: " << ex.what() << '\n';
        answer << GET_INTERLOCUTORS_FAILED << "\n" << END_OF_REQUEST;
    }

    iterator->out_stream_ << answer.str();
    async_write(iterator);
}

void LinkDoveServer::handle_delete_ind_chat(ConnectionIterator iterator) {
    unsigned long long first_id = UtilitySerializator::deserialize_fundamental<unsigned long long>(iterator->in_stream_).second;
    unsigned long long second_id = UtilitySerializator::deserialize_fundamental<unsigned long long>(iterator->in_stream_).second;

    remove_delimeter(iterator);

    std::stringstream answer;
    if (data_base_.delete_ind_chat(first_id, second_id)) {
       answer << DELETE_IND_CHAT_SUCCESS << "\n" << END_OF_REQUEST;
    } else {
        answer << DELETE_IND_CHAT_FAILED << "\n" << END_OF_REQUEST;
    }

    iterator->out_stream_ << answer.str();
    async_write(iterator);
}

void LinkDoveServer::handle_channel_create_request(ConnectionIterator iterator) {
    ChannelInfo channel_info;
    channel_info.deserialize(iterator->in_stream_);

    remove_delimeter(iterator);

    std::stringstream answer;
    if (data_base_.add_channel(channel_info)) {
        answer << CREATE_CHANNEL_SUCCESS << "\n" << END_OF_REQUEST;
    } else {
        answer << CREATE_CHANNEL_FAILED << "\n" << END_OF_REQUEST;
    }

    iterator->out_stream_ << answer.str();
    async_write(iterator);
}

void LinkDoveServer::handle_find_channel_request(ConnectionIterator iterator) {
    std::string channel_name = UtilitySerializator::deserialize_string(iterator->in_stream_).second;

    remove_delimeter(iterator);

    std::stringstream answer;
    try {
        ChannelInfo channel_info;
        channel_info = data_base_.get_channel(channel_name);

        answer << FIND_CHANNEL_SUCCESS << "\n";
        channel_info.serialize(answer);
        answer << END_OF_REQUEST;
    } catch (std::runtime_error& ex) {
        std::cerr << "Failed to find channel: " << ex.what() << '\n';
        answer << FIND_CHANNEL_FAILED << "\n" << END_OF_REQUEST;
    }

    iterator->out_stream_ << answer.str();
    async_write(iterator);
}

void LinkDoveServer::handle_get_channels_request(ConnectionIterator iterator) {
    unsigned long long id = UtilitySerializator::deserialize_fundamental<unsigned long long>(iterator->in_stream_).second;

    remove_delimeter(iterator);

    std::stringstream answer;
    try {
        std::vector<ChannelInfo> channels = data_base_.get_channels(id);

        answer << GET_CHANNELS_SUCCESS << "\n";
        UtilitySerializator::serialize(answer, channels);
        answer << END_OF_REQUEST;
    } catch (std::runtime_error &ex) {
        std::cerr << "Failed to get channels: " << ex.what();
        answer << GET_CHANNELS_FAILED << "\n" << END_OF_REQUEST;
    }

    iterator->out_stream_ << answer.str();
    async_write(iterator);
}

void LinkDoveServer::handle_is_channel_participant_request(ConnectionIterator iterator) {
    unsigned long long user_id    = UtilitySerializator::deserialize_fundamental<unsigned long long>(iterator->in_stream_).second,
                       channel_id = UtilitySerializator::deserialize_fundamental<unsigned long long>(iterator->in_stream_).second;

    remove_delimeter(iterator);

    std::stringstream answer;
    try {
        bool is_participant = data_base_.is_channel_participant(user_id, channel_id);

        answer << IS_CHANNEL_PARTICIPANT_SUCCESS << "\n";
        UtilitySerializator::serialize_fundamental<bool>(answer, is_participant);
        answer << END_OF_REQUEST;
    } catch (std::runtime_error &ex) {
        std::cerr << "Failed to determine channel participant: " << ex.what();
        answer << IS_CHANNEL_PARTICIPANT_FAILED << "\n" << END_OF_REQUEST;
    }

    iterator->out_stream_ << answer.str();
    async_write(iterator);
}

void LinkDoveServer::handle_add_channel_participant_request(ConnectionIterator iterator) {
    unsigned long long user_id    = UtilitySerializator::deserialize_fundamental<unsigned long long>(iterator->in_stream_).second,
                       channel_id = UtilitySerializator::deserialize_fundamental<unsigned long long>(iterator->in_stream_).second;

    remove_delimeter(iterator);

    std::stringstream answer;
    if (data_base_.add_participant_to_channel(user_id, channel_id)) {
        answer << ADD_PARTICIPANT_TO_CHANNEL_SUCCESS << "\n" << END_OF_REQUEST;
    } else {
        answer << ADD_PARTICIPANT_TO_CHANNEL_FAILED << "\n" << END_OF_REQUEST;
    }

    iterator->out_stream_ << answer.str();
    async_write(iterator);
}

void LinkDoveServer::handle_get_channel_messages_request(ConnectionIterator iterator) {
    unsigned long long channel_id = UtilitySerializator::deserialize_fundamental<unsigned long long>(iterator->in_stream_).second;

    remove_delimeter(iterator);

    std::stringstream answer;
    try {
        std::vector<std::shared_ptr<IMessage>> messages = data_base_.get_channel_messages(channel_id);

        // сортируем сообщения по дате
        std::sort(messages.begin(), messages.end(), message_ptr_less_comparator());

        answer << GET_CHNNL_MSG_SUCCESS << "\n";
        UtilitySerializator::serialize(answer, messages);
        answer << END_OF_REQUEST;

    } catch (std::runtime_error& ex) {
        std::cerr << "Failed to get channel messages: " << ex.what() << '\n';
        answer << GET_CHNNL_MSG_FAILED << "\n" << END_OF_REQUEST;
    }

    iterator->out_stream_ << answer.str();
    async_write(iterator);
}

void LinkDoveServer::handle_get_channel_participants_request(ConnectionIterator iterator) {
    unsigned long long channel_id = UtilitySerializator::deserialize_fundamental<unsigned long long>(iterator->in_stream_).second;

    remove_delimeter(iterator);

    std::stringstream answer;
    try {
        std::vector<std::string> participants = data_base_.get_channel_participants(channel_id);

        answer << GET_CHNNL_PARTICIPANTS_SUCCESS << "\n";
        UtilitySerializator::serialize(answer, participants);
        answer << END_OF_REQUEST;
    } catch (std::runtime_error &ex) {
        std::cerr << "Failed to get channel participants: " << ex.what() << '\n';
        answer << GET_CHNNL_PARTICIPANTS_FAILED << "\n" << END_OF_REQUEST;
    }

    iterator->out_stream_ << answer.str();
    async_write(iterator);
}

void LinkDoveServer::handle_delete_channel(ConnectionIterator iterator) {
    unsigned long long channel_id = UtilitySerializator::deserialize_fundamental<unsigned long long>(iterator->in_stream_).second;

    remove_delimeter(iterator);

    std::stringstream answer;
    if (data_base_.delete_channel(channel_id)) {
        answer << DELETE_CHANNEL_SUCCESS << "\n" << END_OF_REQUEST;
    } else {
        answer << DELETE_CHANNEL_FAILED << "\n" << END_OF_REQUEST;
    }

    iterator->out_stream_ << answer.str();
    async_write(iterator);
}

void LinkDoveServer::handle_quit_channel(ConnectionIterator iterator) {
    unsigned long long user_id = UtilitySerializator::deserialize_fundamental<unsigned long long>(iterator->in_stream_).second,
                       channel_id = UtilitySerializator::deserialize_fundamental<unsigned long long>(iterator->in_stream_).second;

    remove_delimeter(iterator);

    std::stringstream answer;
    if (data_base_.quit_channel(user_id, channel_id)) {
        answer << QUIT_CHANNEL_SUCCESS << "\n" << END_OF_REQUEST;
    } else {
        answer << QUIT_CHANNEL_FAILED << "\n" << END_OF_REQUEST;
    }

    iterator->out_stream_ << answer.str();
    async_write(iterator);
}

void LinkDoveServer::handle_remove_user_from_channel(ConnectionIterator iterator) {
    std::string user_name = UtilitySerializator::deserialize_string(iterator->in_stream_).second;
    unsigned long long channel_id = UtilitySerializator::deserialize_fundamental<unsigned long long>(iterator->in_stream_).second;

    std::cerr << user_name << ' ' << channel_id << '\n';

    remove_delimeter(iterator);

    std::stringstream answer;
    try {
        StatusInfo status_info;
        status_info = data_base_.get_status_info(user_name);

        // Запрет на удаление администратора из канала
        if (status_info.id_ != ADMIN_ID && data_base_.quit_channel(status_info.id_, channel_id)) {
            answer << REMOVE_USER_FROM_CHANNEL_SUCCESS << "\n" << END_OF_REQUEST;
        } else {
            answer << REMOVE_USER_FROM_CHANNEL_FAILED << "\n" << END_OF_REQUEST;
        }
    } catch (std::runtime_error &ex) {
        std::cerr << "Failed to remove user from channel: " << ex.what();
        answer << REMOVE_USER_FROM_CHANNEL_FAILED << "\n" << END_OF_REQUEST;
    }

    iterator->out_stream_ << answer.str();
    async_write(iterator);
}

void LinkDoveServer::handle_create_chat_request(ConnectionIterator iterator) {
    ChatInfo chat_info;
    chat_info.deserialize(iterator->in_stream_);

    std::cerr << chat_info.id_ << '\n';

    remove_delimeter(iterator);

    std::stringstream answer;
    if (data_base_.add_chat(chat_info)) {
        answer << CREATE_CHAT_SUCCESS << "\n" << END_OF_REQUEST;
    } else {
        answer << CREATE_CHAT_FAILED << "\n" << END_OF_REQUEST;
    }

    iterator->out_stream_ << answer.str();
    async_write(iterator);
}

void LinkDoveServer::handle_get_chats_request(ConnectionIterator iterator) {
    unsigned long long id = UtilitySerializator::deserialize_fundamental<unsigned long long>(iterator->in_stream_).second;

    remove_delimeter(iterator);

    std::stringstream answer;
    try {
        std::vector<ChatInfo> channels = data_base_.get_chats(id);

        answer << GET_CHATS_SUCCESS << "\n";
        UtilitySerializator::serialize(answer, channels);
        answer << END_OF_REQUEST;
    } catch (std::runtime_error &ex) {
        std::cerr << "Failed to get chats: " << ex.what();
        answer << GET_CHATS_FAILED << "\n" << END_OF_REQUEST;
    }

    iterator->out_stream_ << answer.str();
    async_write(iterator);
}

void LinkDoveServer::handle_find_chat_request(ConnectionIterator iterator) {
    std::string chat_name = UtilitySerializator::deserialize_string(iterator->in_stream_).second;

    remove_delimeter(iterator);

    std::stringstream answer;
    try {
        ChatInfo chat_info;
        chat_info = data_base_.get_chat(chat_name);

        answer << FIND_CHAT_SUCCESS << "\n";
        chat_info.serialize(answer);
        answer << END_OF_REQUEST;
    } catch (std::runtime_error& ex) {
        std::cerr << "Failed to find chat: " << ex.what() << '\n';
        answer << FIND_CHAT_FAILED << "\n" << END_OF_REQUEST;
    }

    iterator->out_stream_ << answer.str();
    async_write(iterator);
}

void LinkDoveServer::handle_is_chat_participant_request(ConnectionIterator iterator) {
    unsigned long long user_id    = UtilitySerializator::deserialize_fundamental<unsigned long long>(iterator->in_stream_).second,
                       chat_id = UtilitySerializator::deserialize_fundamental<unsigned long long>(iterator->in_stream_).second;

    remove_delimeter(iterator);

    std::stringstream answer;
    try {
        bool is_participant = data_base_.is_chat_participant(user_id, chat_id);

        answer << IS_CHAT_PARTICIPANT_SUCCESS << "\n";
        UtilitySerializator::serialize_fundamental<bool>(answer, is_participant);
        answer << END_OF_REQUEST;
    } catch (std::runtime_error &ex) {
        std::cerr << "Failed to determine chat participant: " << ex.what();
        answer << IS_CHAT_PARTICIPANT_FAILED << "\n" << END_OF_REQUEST;
    }

    iterator->out_stream_ << answer.str();
    async_write(iterator);
}

void LinkDoveServer::handle_add_chat_participant_request(ConnectionIterator iterator) {
    unsigned long long user_id    = UtilitySerializator::deserialize_fundamental<unsigned long long>(iterator->in_stream_).second,
                       chat_id = UtilitySerializator::deserialize_fundamental<unsigned long long>(iterator->in_stream_).second;

    remove_delimeter(iterator);

    std::stringstream answer;
    if (data_base_.add_participant_to_chat(user_id, chat_id)) {
        answer << ADD_PARTICIPANT_TO_CHAT_SUCCESS << "\n" << END_OF_REQUEST;
    } else {
        answer << ADD_PARTICIPANT_TO_CHAT_FAILED << "\n" << END_OF_REQUEST;
    }

    iterator->out_stream_ << answer.str();
    async_write(iterator);
}

void LinkDoveServer::handle_get_chat_messages_request(ConnectionIterator iterator) {
    unsigned long long chat_id = UtilitySerializator::deserialize_fundamental<unsigned long long>(iterator->in_stream_).second;

    remove_delimeter(iterator);
    std::cerr << "here\n";

    std::stringstream answer;
    try {
        std::vector<std::shared_ptr<IMessage>> messages = data_base_.get_chat_messages(chat_id);

        // сортируем сообщения по дате
        std::sort(messages.begin(), messages.end(), message_ptr_less_comparator());

        answer << GET_CHAT_MSG_SUCCESS << "\n";
        UtilitySerializator::serialize(answer, messages);
        answer << END_OF_REQUEST;

    } catch (std::runtime_error& ex) {
        std::cerr << "Failed to get chat messages:" << ex.what() << '\n';
        answer << GET_CHAT_MSG_FAILED << "\n" << END_OF_REQUEST;
    }

    iterator->out_stream_ << answer.str();
    async_write(iterator);
}

void LinkDoveServer::handle_get_chat_participants_request(ConnectionIterator iterator) {
    unsigned long long group_id = UtilitySerializator::deserialize_fundamental<unsigned long long>(iterator->in_stream_).second;

    remove_delimeter(iterator);

    std::stringstream answer;
    try {
        std::vector<std::string> participants = data_base_.get_chat_participants(group_id);

        answer << GET_CHAT_PARTICIPANTS_SUCCESS << "\n";
        UtilitySerializator::serialize(answer, participants);
        answer << END_OF_REQUEST;
    } catch (std::runtime_error &ex) {
        std::cerr << "Failed to get chat participants: " << ex.what() << '\n';
        answer << GET_CHAT_PARTICIPANTS_FAILED << "\n" << END_OF_REQUEST;
    }

    iterator->out_stream_ << answer.str();
    async_write(iterator);
}

void LinkDoveServer::handle_delete_chat(ConnectionIterator iterator) {
    unsigned long long chat_id = UtilitySerializator::deserialize_fundamental<unsigned long long>(iterator->in_stream_).second;

    remove_delimeter(iterator);

    std::stringstream answer;
    if (data_base_.delete_chat(chat_id)) {
        answer << DELETE_CHAT_SUCCESS << "\n" << END_OF_REQUEST;
    } else {
        answer << DELETE_CHAT_FAILED << "\n" << END_OF_REQUEST;
    }

    iterator->out_stream_ << answer.str();
    async_write(iterator);
}

void LinkDoveServer::handle_quit_chat(ConnectionIterator iterator) {
    unsigned long long user_id = UtilitySerializator::deserialize_fundamental<unsigned long long>(iterator->in_stream_).second,
                       chat_id = UtilitySerializator::deserialize_fundamental<unsigned long long>(iterator->in_stream_).second;

    remove_delimeter(iterator);

    std::stringstream answer;
    if (data_base_.quit_chat(user_id, chat_id)) {
        answer << QUIT_CHAT_SUCCESS << "\n" << END_OF_REQUEST;
    } else {
        answer << QUIT_CHAT_FAILED << "\n" << END_OF_REQUEST;
    }

    iterator->out_stream_ << answer.str();
    async_write(iterator);
}

void LinkDoveServer::handle_remove_user_from_chat(ConnectionIterator iterator) {
    std::string user_name = UtilitySerializator::deserialize_string(iterator->in_stream_).second;
    unsigned long long group_id = UtilitySerializator::deserialize_fundamental<unsigned long long>(iterator->in_stream_).second;

    remove_delimeter(iterator);

    std::stringstream answer;
    try {
        StatusInfo status_info;
        status_info = data_base_.get_status_info(user_name);

        // Запрет на удаление администратора из чата
        if (status_info.id_ != ADMIN_ID && data_base_.quit_chat(status_info.id_, group_id)) {
            answer << REMOVE_USER_FROM_CHAT_SUCCESS << "\n" << END_OF_REQUEST;
        } else {
            answer << REMOVE_USER_FROM_CHAT_FAILED << "\n" << END_OF_REQUEST;
        }
    } catch (std::runtime_error &ex) {
        std::cerr << "Failed to remove user from chat: " << ex.what();
        answer << REMOVE_USER_FROM_CHAT_FAILED << "\n" << END_OF_REQUEST;
    }

    iterator->out_stream_ << answer.str();
    async_write(iterator);
}

void LinkDoveServer::handle_answer_user(ConnectionIterator iterator) {
    unsigned long long user_id = UtilitySerializator::deserialize_fundamental<unsigned long long>(iterator->in_stream_).second;
    std::string text = UtilitySerializator::deserialize_string(iterator->in_stream_).second;

    remove_delimeter(iterator);

    std::stringstream answer;
    if (data_base_.add_answer(user_id, text)) {
        answer << SEND_USER_ANSWER_SUCCESS << "\n" << END_OF_REQUEST;
    } else {
        answer << SEND_USER_ANSWER_FAILED << "\n" << END_OF_REQUEST;
    }

    iterator->out_stream_ << answer.str();
    async_write(iterator);
}

void LinkDoveServer::handle_get_notifications(ConnectionIterator iterator) {
    unsigned long long user_id = UtilitySerializator::deserialize_fundamental<unsigned long long>(iterator->in_stream_).second;

    remove_delimeter(iterator);

    std::stringstream answer;
    try {
        std::vector<Notification> notifications = data_base_.get_notifications(user_id, GET_NOTIFICATIONS_LIMIT);
        std::cerr << notifications.size() << '\n';

        answer << GET_NOTIFICATIONS_SUCCESS << "\n";
        UtilitySerializator::serialize(answer, notifications);
        answer << END_OF_REQUEST;
    } catch (std::runtime_error &ex) {
        std::cerr << ex.what() << '\n';
        answer << GET_NOTIFICATIONS_FAILED << "\n" << END_OF_REQUEST;
    }

    iterator->out_stream_ << answer.str();
    async_write(iterator);
}

void LinkDoveServer::handle_del_notification(ConnectionIterator iterator) {
    unsigned long long id = UtilitySerializator::deserialize_fundamental<unsigned long long>(iterator->in_stream_).second;

    remove_delimeter(iterator);

    std::stringstream answer;
    if (data_base_.delete_notification(id)) {
        answer << DEL_NOTIFICATION_SUCCESS << "\n" << END_OF_REQUEST;
    } else {
        answer << DEL_NOTIFICATION_FAILED << "\n" << END_OF_REQUEST;
    }

    iterator->out_stream_ << answer.str();
    async_write(iterator);
}

void LinkDoveServer::handle_delete_msg(ConnectionIterator iterator) {
    std::shared_ptr<IMessage> msg_ptr = UtilitySerializator::deserialize_msg(iterator->in_stream_).second;

    remove_delimeter(iterator);

    std::stringstream answer;
    switch (msg_ptr->get_msg_type()) {
        case INDIVIDUAL_MSG_TYPE: {
            if (data_base_.delete_ind_message(*msg_ptr)) {
                answer << DEL_IND_MSG_SUCCESS << "\n" << END_OF_REQUEST;
            } else {
                answer << DEL_IND_MSG_FAILED << "\n" << END_OF_REQUEST;
            }

            break;
        }
        case CHANNEL_MSG_TYPE: {
            if (data_base_.delete_chnnl_message(*msg_ptr)) {
                answer << DEL_CHANNEL_MSG_SUCCESS << "\n" << END_OF_REQUEST;
            } else {
                answer << DEL_CHANNEL_MSG_FAILED << "\n" << END_OF_REQUEST;
            }

            break;
        }
        case GROUP_MSG_TYPE: {
            if (data_base_.delete_chat_message(*msg_ptr)) {
                answer << DEL_CHANNEL_MSG_SUCCESS << "\n" << END_OF_REQUEST;
            } else {
                answer << DEL_CHANNEL_MSG_FAILED << "\n" << END_OF_REQUEST;
            }

            break;
        }
    }

    iterator->out_stream_ << answer.str();
    async_write(iterator);
}

void LinkDoveServer::run_context() {
    std::thread t([&]() {
        if (io_context_ptr_->stopped()) {
            io_context_ptr_->restart();
        }

        io_context_ptr_->run();
    });

    t.detach();
}

void LinkDoveServer::remove_delimeter(ConnectionIterator iterator) {
    iterator->buffer_.consume(sizeof(END_OF_REQUEST));
}
