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
    //iterator->out_stream_ << "CLOSE\n";
    asio::async_write(iterator->socket_, iterator->buffer_,
                      boost::bind(&LinkDoveServer::handle_async_write,
                                  shared_from_this(),
                                  iterator,
                                  asio::placeholders::error,
                                  asio::placeholders::bytes_transferred));

    run_context();
}

void LinkDoveServer::handle_async_accept(ConnectionIterator iterator, boost::system::error_code error) {
    if (error) {
        std::cerr << "Failed to accept new connection: " << error.message() << '\n';
        async_close_write(iterator);
    } else {
        std::cout << "Connection from: " << iterator->socket_.remote_endpoint().address() << "\n";
    }

    async_read(iterator);
    start_async_accept();
}

void LinkDoveServer::handle_async_read(ConnectionIterator iterator, boost::system::error_code error, size_t bytes_transfered) {
    if (error) {
        std::cerr << "Failed to read from socket: " << error.value() << ' ' << error.message() << '\n';
        async_close_write(iterator);
    }

    if (bytes_transfered > 0) {
        handle_type_request(iterator);
    }
}

void LinkDoveServer::handle_async_write(ConnectionIterator iterator, boost::system::error_code error, size_t bytes_transfered) {
    if (error) {
        std::cerr << "Failed to write to socket: " << error.value() << ' ' << error.message() << '\n';
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

    if (request_type == LOGIN_REQUEST) {
        handle_login_request(iterator);
    } else if (request_type == REGISTER_REQUEST) {
        handle_register_request(iterator);
    } else if (request_type == SEND_COMPLAINT_REQUEST) {
        handle_send_complaint_request(iterator);
    } else if (request_type == DEL_COMPLAINT_REQUEST) {
        handle_del_complaint_request(iterator);
    } else if (request_type == GET_COMPLAINTS_REQUEST) {
        handle_get_complaints_request(iterator);
    } else if (request_type == UPDATE_USER_REQUEST) {
        handle_update_user_request(iterator);
    } else if (request_type == FIND_USER_REQUEST) {
        handle_find_user_request(iterator);
    } else if (request_type == BAN_USER_REQUEST) {
        handle_ban_user_request(iterator);
    } else if (request_type == SEND_MSG_REQUEST) {
        handle_send_msg_request(iterator);
    } else if (request_type == GET_IND_MSG_REQUEST) {
        handle_get_msg_request(iterator);
    } else if (request_type == GET_INTERLOCUTORS_REQUEST) {
        handle_get_interlocutors_request(iterator);
    } else if (request_type == CREATE_CHANNEL_REQUEST) {
        handle_channel_create_request(iterator);
    } else if (request_type == FIND_CHANNEL_REQUEST) {
        handle_find_channel_request(iterator);
    } else if (request_type == GET_CHANNELS_REQUEST) {
        handle_get_channels_request(iterator);
    } else if (request_type == IS_CHANNEL_PARTICIPANT_REQUEST) {
        handle_is_channel_participant_request(iterator);
    } else if (request_type == ADD_PARTICIPANT_TO_CHANNEL_REQUEST) {
        handle_add_channel_participant_request(iterator);
    } else if (request_type == GET_CHNNL_MSG_REQUEST) {
        std::cerr << "get channel messages\n";
        handle_get_channel_messages_request(iterator);
    }
}

void LinkDoveServer::handle_login_request(ConnectionIterator iterator) {
    LoginInfo login_info;
    login_info.deserialize(iterator->in_stream_);

    remove_delimeter(iterator);

    if (data_base_.login_user(login_info)) {
        try {
            if (data_base_.get_user_banned_status(login_info.username_)) {
                iterator->out_stream_ << LOGIN_BANNED << "\n";
            } else {

                iterator->out_stream_ << LOGIN_SUCCESS << "\n";

                // ДОБАВИТЬ TRY_CATCH (С ОТПРАВКОЙ КЛИЕНТУ INTERNAL_ERROR)
                StatusInfo status_info = data_base_.get_status_info(login_info.username_);
                status_info.serialize(iterator->out_stream_);
            }

        } catch (std::runtime_error &error) {
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
            std::cerr << ex.what() << '\n';
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
        std::cerr << ex.what() << '\n';
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
        std::cerr << ex.what() << '\n';
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
        answer << GET_INTERLOCUTORS_FAILED << "\n" << END_OF_REQUEST;
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
        std::cerr << ex.what() << '\n';
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
        std::cerr << ex.what();
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
        std::cerr << ex.what();
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
        std::cerr << ex.what() << '\n';
        answer << GET_CHNNL_MSG_FAILED << "\n" << END_OF_REQUEST;
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
