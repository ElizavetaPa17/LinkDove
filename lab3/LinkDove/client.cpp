#include "client.h"

#include <sstream>
#include <iostream>
#include <QDate>

#include "utility.h"

Client::Client(std::shared_ptr<asio::io_context> io_context_ptr, boost::asio::ip::address address, uint16_t port)
    : io_context_ptr_(io_context_ptr),
      connection_(io_context_ptr_),
      endpoint_(address, port),
      is_connected_(false)
{

}

// лучше io_context в отдельный класс вынести и там запустить его работу в отдельном потоке
void Client::async_connect() {
    connection_.socket_.async_connect(endpoint_,
                                      boost::bind(&Client::handle_async_connect,
                                                  shared_from_this(),
                                                  asio::placeholders::error));

    run_context();
}

void Client::async_login(const LoginInfo& login_info) {
   connection_.out_stream_ << create_login_request(login_info);

   asio::async_write(connection_.socket_, connection_.buffer_,
                     boost::bind(&Client::handle_async_write,
                                 shared_from_this(),
                                 asio::placeholders::error,
                                 asio::placeholders::bytes_transferred));

   run_context();
}

void Client::async_register(const UserInfo& user_info) {
    connection_.out_stream_ << create_register_request(user_info);
    status_info_ = user_info.status_info_;

    asio::async_write(connection_.socket_, connection_.buffer_,
                      boost::bind(&Client::handle_async_write,
                                  shared_from_this(),
                                  asio::placeholders::error,
                                  asio::placeholders::bytes_transferred));
    run_context();
}

void Client::async_send_complaint(const std::string& text) {
    Complaint complaint;
    complaint.sender_id_ = status_info_.id_;
    complaint.text_ = text;

    connection_.out_stream_ << create_send_complaint_request(complaint);

    asio::async_write(connection_.socket_, connection_.buffer_,
                      boost::bind(&Client::handle_async_write,
                                  shared_from_this(),
                                  asio::placeholders::error,
                                  asio::placeholders::bytes_transferred));

    run_context();
}
// ВЕЗДЕ СОЗДАТЬ ФУНКЦИИ СОЗДАНИЯ ЗАПРОСА!
// выделить отдельный stringstream, чтобы каждый раз не создавать на стеке новый
// вынести async_write в отдельную функцию
void Client::async_del_complaint(unsigned long long complaint_id) {
    connection_.out_stream_ << DEL_COMPLAINT_REQUEST << "\n";
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, complaint_id);
    connection_.out_stream_ << END_OF_REQUEST;

    asio::async_write(connection_.socket_, connection_.buffer_,
                      boost::bind(&Client::handle_async_write,
                                  shared_from_this(),
                                  asio::placeholders::error,
                                  asio::placeholders::bytes_transferred));

    run_context();
}

void Client::async_get_complaints() {
    connection_.out_stream_ << GET_COMPLAINTS_REQUEST << "\n" << END_OF_REQUEST;

    asio::async_write(connection_.socket_, connection_.buffer_,
                      boost::bind(&Client::handle_async_write,
                                  shared_from_this(),
                                  asio::placeholders::error,
                                  asio::placeholders::bytes_transferred));

    run_context();
}

void Client::async_update_user(StatusInfo& status_info) {
    // Идентификатор пользователя известен только классу Client
    // Поэтому status_info не является константным, т.к. нам необходимо установить поле идентификатора.
    updated_status_info_ = status_info;
    updated_status_info_.id_ = status_info_.id_;
    connection_.out_stream_ << create_update_user_request(updated_status_info_);

    asio::async_write(connection_.socket_, connection_.buffer_,
                      boost::bind(&Client::handle_async_write,
                                  shared_from_this(),
                                  asio::placeholders::error,
                                  asio::placeholders::bytes_transferred));

    run_context();
}

void Client::async_find_user(const std::string &username) {
    connection_.out_stream_ << FIND_USER_REQUEST << "\n";
    UtilitySerializator::serialize(connection_.out_stream_, username);
    connection_.out_stream_ << END_OF_REQUEST;

    asio::async_write(connection_.socket_, connection_.buffer_,
                      boost::bind(&Client::handle_async_write,
                                  shared_from_this(),
                                  asio::placeholders::error,
                                  asio::placeholders::bytes_transferred));

    run_context();
}

void Client::async_ban_user(const std::string &username, bool is_ban) {
    connection_.out_stream_ << create_ban_user_request(username, is_ban);

    asio::async_write(connection_.socket_, connection_.buffer_,
                      boost::bind(&Client::handle_async_write,
                                  shared_from_this(),
                                  asio::placeholders::error,
                                  asio::placeholders::bytes_transferred));

    run_context();
}

void Client::async_send_message(const IMessage& message) {
    connection_.out_stream_ << SEND_MSG_REQUEST << "\n";
    UtilitySerializator::serialize(connection_.out_stream_, message);
    connection_.out_stream_ << END_OF_REQUEST;

    asio::async_write(connection_.socket_, connection_.buffer_,
                      boost::bind(&Client::handle_async_write,
                                  shared_from_this(),
                                  asio::placeholders::error,
                                  asio::placeholders::bytes_transferred));

    run_context();
}

void Client::async_get_ind_messages(unsigned long long other_id) {
    connection_.out_stream_ << create_get_ind_message_request(other_id);

    asio::async_write(connection_.socket_, connection_.buffer_,
                      boost::bind(&Client::handle_async_write,
                                  shared_from_this(),
                                  asio::placeholders::error,
                                  asio::placeholders::bytes_transferred));

    run_context();
}

void Client::async_get_interlocutors() {
    connection_.out_stream_ << GET_INTERLOCUTORS_REQUEST << "\n";
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, status_info_.id_);
    connection_.out_stream_ << END_OF_REQUEST;

    asio::async_write(connection_.socket_, connection_.buffer_,
                      boost::bind(&Client::handle_async_write,
                                  shared_from_this(),
                                  asio::placeholders::error,
                                  asio::placeholders::bytes_transferred));

    run_context();
}

void Client::async_create_channel(const std::string &channel_name) {
    connection_.out_stream_ << CREATE_CHANNEL_REQUEST << "\n";

    ChannelInfo channel_info;
    channel_info.name_ = channel_name;
    channel_info.owner_id_ = status_info_.id_;
    channel_info.serialize(connection_.out_stream_);

    connection_.out_stream_ << END_OF_REQUEST;

    asio::async_write(connection_.socket_, connection_.buffer_,
                      boost::bind(&Client::handle_async_write,
                                  shared_from_this(),
                                  asio::placeholders::error,
                                  asio::placeholders::bytes_transferred));

    run_context();
}

void Client::async_find_channel(const std::string &channel_name) {
    connection_.out_stream_ << FIND_CHANNEL_REQUEST << "\n";
    UtilitySerializator::serialize(connection_.out_stream_, channel_name);
    connection_.out_stream_ << END_OF_REQUEST;

    asio::async_write(connection_.socket_, connection_.buffer_,
                      boost::bind(&Client::handle_async_write,
                                  shared_from_this(),
                                  asio::placeholders::error,
                                  asio::placeholders::bytes_transferred));

    run_context();
}

void Client::async_get_channels() {
    connection_.out_stream_ << GET_CHANNELS_REQUEST << "\n";
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, status_info_.id_);
    connection_.out_stream_ << END_OF_REQUEST;


    asio::async_write(connection_.socket_, connection_.buffer_,
                       boost::bind(&Client::handle_async_write,
                                   shared_from_this(),
                                   asio::placeholders::error,
                                   asio::placeholders::bytes_transferred));

     run_context();
}

// ПОВТОРЕНИЕ КОДА С ФУНКЦИЕЙ НИЖЕ
void Client::async_is_channel_participant_request(unsigned long long channel_id) {
    connection_.out_stream_ << IS_CHANNEL_PARTICIPANT_REQUEST << "\n";
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, status_info_.id_);
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, channel_id);
    connection_.out_stream_ << END_OF_REQUEST;

    asio::async_write(connection_.socket_, connection_.buffer_,
                       boost::bind(&Client::handle_async_write,
                                   shared_from_this(),
                                   asio::placeholders::error,
                                   asio::placeholders::bytes_transferred));

    run_context();
}

void Client::async_add_channel_participant_request(unsigned long long channel_id) {
    connection_.out_stream_ << ADD_PARTICIPANT_TO_CHANNEL_REQUEST << "\n";
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, status_info_.id_);
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, channel_id);
    connection_.out_stream_ << END_OF_REQUEST;

    asio::async_write(connection_.socket_, connection_.buffer_,
                       boost::bind(&Client::handle_async_write,
                                   shared_from_this(),
                                   asio::placeholders::error,
                                   asio::placeholders::bytes_transferred));

    run_context();
}

void Client::async_get_channel_messages(unsigned long long channel_id) {
    connection_.out_stream_ << GET_CHNNL_MSG_REQUEST << "\n";
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, channel_id);
    connection_.out_stream_ << END_OF_REQUEST;

    asio::async_write(connection_.socket_, connection_.buffer_,
                       boost::bind(&Client::handle_async_write,
                                   shared_from_this(),
                                   asio::placeholders::error,
                                   asio::placeholders::bytes_transferred));

    run_context();
}

void Client::async_delete_channel(unsigned long long channel_id) {
    connection_.out_stream_ << DELETE_CHANNEL_REQUEST << "\n";
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, channel_id);
    connection_.out_stream_ << END_OF_REQUEST;

    asio::async_write(connection_.socket_, connection_.buffer_,
                       boost::bind(&Client::handle_async_write,
                                   shared_from_this(),
                                   asio::placeholders::error,
                                   asio::placeholders::bytes_transferred));

    run_context();
}

void Client::async_create_chat(const std::string &chat_name) {
    connection_.out_stream_ << CREATE_CHAT_REQUEST << "\n";

    ChatInfo chat_info;
    chat_info.name_ = chat_name;
    chat_info.owner_id_ = status_info_.id_;
    chat_info.serialize(connection_.out_stream_);

    connection_.out_stream_ << END_OF_REQUEST;

    asio::async_write(connection_.socket_, connection_.buffer_,
                      boost::bind(&Client::handle_async_write,
                                  shared_from_this(),
                                  asio::placeholders::error,
                                  asio::placeholders::bytes_transferred));

    run_context();
}

void Client::async_get_chats() {
    connection_.out_stream_ << GET_CHATS_REQUEST << "\n";
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, status_info_.id_);
    connection_.out_stream_ << END_OF_REQUEST;


    asio::async_write(connection_.socket_, connection_.buffer_,
                       boost::bind(&Client::handle_async_write,
                                   shared_from_this(),
                                   asio::placeholders::error,
                                   asio::placeholders::bytes_transferred));

     run_context();
}

void Client::async_find_chat(const std::string &chat_name) {
    connection_.out_stream_ << FIND_CHAT_REQUEST << "\n";
    UtilitySerializator::serialize(connection_.out_stream_, chat_name);
    connection_.out_stream_ << END_OF_REQUEST;

    asio::async_write(connection_.socket_, connection_.buffer_,
                      boost::bind(&Client::handle_async_write,
                                  shared_from_this(),
                                  asio::placeholders::error,
                                  asio::placeholders::bytes_transferred));

    run_context();
}

void Client::   async_is_chat_participant_request(unsigned long long chat_id) {
    connection_.out_stream_ << IS_CHAT_PARTICIPANT_REQUEST << "\n";
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, status_info_.id_);
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, chat_id);
    connection_.out_stream_ << END_OF_REQUEST;

    asio::async_write(connection_.socket_, connection_.buffer_,
                       boost::bind(&Client::handle_async_write,
                                   shared_from_this(),
                                   asio::placeholders::error,
                                   asio::placeholders::bytes_transferred));

    run_context();
}

void Client::async_add_chat_participant_request(unsigned long long chat_id) {
    connection_.out_stream_ << ADD_PARTICIPANT_TO_CHAT_REQUEST << "\n";
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, status_info_.id_);
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, chat_id);
    connection_.out_stream_ << END_OF_REQUEST;

    asio::async_write(connection_.socket_, connection_.buffer_,
                       boost::bind(&Client::handle_async_write,
                                   shared_from_this(),
                                   asio::placeholders::error,
                                   asio::placeholders::bytes_transferred));

    run_context();
}

void Client::async_get_chat_messages(unsigned long long chat_id) {
    connection_.out_stream_ << GET_CHAT_MSG_REQUEST << "\n";
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, chat_id);
    connection_.out_stream_ << END_OF_REQUEST;

    asio::async_write(connection_.socket_, connection_.buffer_,
                       boost::bind(&Client::handle_async_write,
                                   shared_from_this(),
                                   asio::placeholders::error,
                                   asio::placeholders::bytes_transferred));

    run_context();

    std::cerr << "send\n";
}

StatusInfo Client::get_status_info() {
    return status_info_;
}

StatusInfo Client::get_found_user() {
    return found_status_info_;
}

std::vector<Complaint> Client::get_complaints() {
    return complaints_;
}

std::vector<std::shared_ptr<IMessage>> Client::get_messages() {
    return messages_;
}

std::vector<StatusInfo> Client::get_interlocutors() {
    return status_info_vec_;
}

ChannelInfo Client::get_found_channel() {
    return found_channel_info_;
}

std::vector<ChannelInfo> Client::get_channels() {
    return channels_;
}

std::vector<ChatInfo> Client::get_chats() {
    return chats_;
}

ChatInfo Client::get_found_chat() {
    return found_chat_info_;
}

bool Client::is_connected() noexcept {
    return is_connected_;
}

std::string Client::create_login_request(const LoginInfo& login_info) {
    std::stringstream str_stream;
    str_stream << LOGIN_REQUEST << "\n";

    login_info.serialize(str_stream) << '\n';
    str_stream << END_OF_REQUEST;

    return str_stream.str();
}

std::string Client::create_register_request(const UserInfo& user_info) {
    std::stringstream str_stream;
    str_stream << REGISTER_REQUEST << '\n';

    user_info.serialize(str_stream);
    str_stream << END_OF_REQUEST;

    return str_stream.str();
}

std::string Client::create_send_complaint_request(const Complaint& complaint) {
    std::stringstream str_stream;
    str_stream << SEND_COMPLAINT_REQUEST << '\n';

    complaint.serialize(str_stream);
    str_stream << END_OF_REQUEST;

    return str_stream.str();
}

std::string Client::create_del_request(unsigned long long complaint_id) {
    std::stringstream str_stream;

    str_stream << DEL_COMPLAINT_REQUEST << '\n';
    UtilitySerializator::serialize_fundamental(str_stream, complaint_id);
    str_stream << END_OF_REQUEST;

    return str_stream.str();
}

std::string Client::create_update_user_request(const StatusInfo& status_info) {
    std::stringstream str_stream;
    str_stream << UPDATE_USER_REQUEST << '\n';

    status_info.serialize(str_stream);
    str_stream << END_OF_REQUEST;

    return str_stream.str();
}

std::string Client::create_ban_user_request(const std::string &username, bool is_ban) {
    std::stringstream str_stream;
    str_stream << BAN_USER_REQUEST << '\n';

    UtilitySerializator::serialize(str_stream, username);
    UtilitySerializator::serialize_fundamental<uint8_t>(str_stream, is_ban);
    str_stream << END_OF_REQUEST;

    return str_stream.str();
}

std::string Client::create_get_ind_message_request(unsigned long long other_id) {
    std::stringstream str_stream;
    str_stream << GET_IND_MSG_REQUEST << '\n';

    UtilitySerializator::serialize_fundamental<unsigned long long>(str_stream, status_info_.id_);
    UtilitySerializator::serialize_fundamental<unsigned long long>(str_stream, other_id);

    str_stream << END_OF_REQUEST;

    return str_stream.str();
}

void Client::async_read() {
    asio::async_read_until(connection_.socket_,
                           connection_.buffer_,
                           END_OF_REQUEST,
                           boost::bind(&Client::handle_async_read,
                                 shared_from_this(),
                                 asio::placeholders::error(),
                                 asio::placeholders::bytes_transferred()));

    run_context();
}

void Client::handle_async_connect(boost::system::error_code error) {
    if (error) {
        std::cerr << "Failed to connect: " << error.value() << ' ' << error.message() << '\n';
        is_connected_ = false;
        throw std::runtime_error("Cannot connect to the server");
    } else {
        is_connected_ = true;
        std::cerr << "Successfull connection to the server.\n";
    }
}


void Client::handle_async_write(boost::system::error_code error, size_t bytes_transferred) {
    if (error) {
        std::cerr << "Failed to send the request: " << error.value() << ' ' << error.message() << '\n';
        throw std::runtime_error("Failed to send register request");
    }


    if (bytes_transferred > 0) {
        std::cerr << "Send the request to the server. Transfer " << bytes_transferred << " bytes.\n";
        async_read();
    }
}

void Client::handle_async_read(boost::system::error_code error, size_t bytes_transferred) {
    if (error) {
        std::cerr << "Failed to send register request: " << error.value() << ' ' << error.message() << '\n';
        // ЗАКРЫВАТЬ СОЕДИНЕНИЕ?
    }


    if (bytes_transferred > 0) {
        std::string answer_type;
        std::getline(connection_.in_stream_, answer_type);

        std::cerr << "answer: " << answer_type << '\n';

        // вместо гигантского else if прикрути MAP!!!1!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        if (answer_type == LOGIN_SUCCESS) {
            status_info_.deserialize(connection_.in_stream_);
            emit authorization_result(LOGIN_SUCCESS_ANSWER);
        } else if (answer_type == LOGIN_BANNED) {
            emit authorization_result(LOGIN_BANNED_ANSWER);
        } else if (answer_type == LOGIN_FAILED) {
            emit authorization_result(LOGIN_FAILED_ANSWER);
        } else if (answer_type == REGISTER_SUCCESS) {
            status_info_.deserialize(connection_.in_stream_);
            emit authorization_result(REGISTRATION_SUCCESS_ANSWER);
        } else if (answer_type == REGISTER_FAILED) {
            emit authorization_result(REGISTRATION_FAILED_ANSWER);
        } else if (answer_type == SEND_COMPLAINT_SUCCESS) {
            emit send_complaint_result(SEND_COMPLAINT_SUCCESS_ANSWER);
        } else if (answer_type == SEND_COMPLAINT_FAILED){
            emit send_complaint_result(SEND_COMPLAINT_FAILED_ANSWER);
        } else if (answer_type == DEL_COMPLAINT_SUCCESS) {
            emit del_complaint_result(DEL_COMPLAINT_SUCCESS_ANSWER);
        } else if (answer_type == DEL_COMPLAINT_FAILED) {
            emit del_complaint_result(DEL_COMPLAINT_FAILED_ANSWER);
        } else if (answer_type == GET_COMPLAINTS_SUCCESS) {
            complaints_ = UtilitySerializator::deserialize_compl_vec(connection_.in_stream_).second;
            emit get_complaints_result(GET_COMPLAINTS_SUCCESS_ANSWER);
        } else if (answer_type == GET_COMPLAINTS_FAILED) {
            emit get_complaints_result(GET_COMPLAINTS_FAILED_ANSWER);
        } else if (answer_type == UPDATE_USER_SUCCESS) {
            status_info_ = updated_status_info_;
            emit update_user_result(UPDATE_USER_SUCCESS_ANSWER);
        } else if (answer_type == UPDATE_USER_FAILED){
            emit update_user_result(UPDATE_USER_FAILED_ANSWER);
        } else if (answer_type == FIND_USER_SUCCESS) {
            found_status_info_.deserialize(connection_.in_stream_);
            emit find_user_result(FIND_USER_SUCCESS_ANSWER);
        } else if (answer_type == FIND_USER_FAILED) {
            emit find_user_result(FIND_USER_FAILED_ANWSER);
        } else if (answer_type == BAN_USER_SUCCESS) {
            emit ban_user_result(BAN_USER_SUCCESS_ANSWER);
        } else if (answer_type == BAN_USER_FAILED){
            emit ban_user_result(BAN_USER_FAILED_ANSWER);
        } else if (answer_type == SEND_IND_MSG_SUCCESS) {
            emit send_msg_result(SEND_IND_MSG_SUCCESS_ANSWER);
        } else if (answer_type == SEND_IND_MSG_FAILED) {
            emit send_msg_result(SEND_IND_MSG_FAILED_ANSWER);
        } else if (answer_type ==  GET_IND_MSG_SUCCESS) {
            messages_ = UtilitySerializator::deserialize_msg_vec(connection_.in_stream_).second;
            emit get_ind_msg_result(GET_IND_MSG_SUCCESS_ANSWER);
        } else if (answer_type == GET_IND_MSG_FAILED) {
            emit get_ind_msg_result(GET_IND_MSG_FAILED_ANSWER);
        } else if (answer_type == GET_INTERLOCUTORS_SUCCESS) {
            status_info_vec_ = UtilitySerializator::deserialize_st_info_vec(connection_.in_stream_).second;
            emit get_interlocutors_result(GET_INTERLOCUTORS_SUCCESS_ANSWER);
        } else if (answer_type == GET_INTERLOCUTORS_FAILED) {
            emit get_interlocutors_result(GET_INTERLOCUTORS_FAILED_ANSWER);
        } else if (answer_type == CREATE_CHANNEL_SUCCESS) {
            emit create_channel_result(CREATE_CHANNEL_SUCCESS_ANSWER);
        } else if (answer_type == CREATE_CHANNEL_FAILED) {
            emit create_channel_result(CREATE_CHANNEL_FAILED_ANSWER);
        } else if (answer_type == FIND_CHANNEL_SUCCESS) {
            emit find_channel_result(FIND_CHANNEL_SUCCESS_ANSWER);
            found_channel_info_.deserialize(connection_.in_stream_);
        } else if (answer_type == FIND_CHANNEL_FAILED){
            emit find_channel_result(FIND_CHANNEL_FAILED_ANSWER);
        } else if (answer_type == GET_CHANNELS_SUCCESS) {
            emit get_channels_result(GET_CHANNELS_SUCCESS_ANSWER);
            channels_ = UtilitySerializator::deserialize_ch_info_vec(connection_.in_stream_).second;
        } else if (answer_type == GET_CHANNELS_FAILED) {
            emit get_channels_result(GET_CHANNELS_FAILED_ANSWER);
        } else if (answer_type == IS_CHANNEL_PARTICIPANT_SUCCESS) {
            bool is_participant = UtilitySerializator::deserialize_fundamental<bool>(connection_.in_stream_).second;
            emit is_channel_participant_result(IS_CHANNEL_PARTICIPANT_SUCCESS_ANSWER, is_participant);
        } else if (answer_type == IS_CHANNEL_PARTICIPANT_FAILED) {
            emit is_channel_participant_result(IS_CHANNEL_PARTICIPANT_FAILED_ANSWER, false);
        } else if (answer_type == ADD_PARTICIPANT_TO_CHANNEL_SUCCESS) {
            emit add_participant_to_channel_result(ADD_PARTICIPANT_TO_CHANNEL_SUCCESS_ANSWER);
        } else if (answer_type == ADD_PARTICIPANT_TO_CHANNEL_FAILED) {
            emit add_participant_to_channel_result(ADD_PARTICIPANT_TO_CHANNEL_FAILED_ANSWER);
        } else if (answer_type == SEND_CHNNL_MSG_SUCCESS) {
            emit send_msg_result(SEND_CHNNL_MSG_SUCCESS_ANSWER);
        } else if (answer_type == SEND_CHNNL_MSG_FAILED) {
            emit send_msg_result(SEND_CHNNL_MSG_FAILED_ANSWER);
        } else if (answer_type == GET_CHNNL_MSG_SUCCESS) {
            messages_ = UtilitySerializator::deserialize_msg_vec(connection_.in_stream_).second;
            emit get_channel_msg_result(GET_CHNNL_MSG_SUCCESS_ANSWER);
        } else if (answer_type == GET_CHNNL_MSG_FAILED){
            emit get_channel_msg_result(GET_CHNNL_MSG_FAILED_ANSWER);
        } else if (answer_type == CREATE_CHAT_SUCCESS) {
            emit create_chat_result(CREATE_CHAT_SUCCESS_ANSWER);
        } else if (answer_type == CREATE_CHAT_FAILED) {
            emit create_chat_result(CREATE_CHAT_FAILED_ANSWER);
        } else if (answer_type == GET_CHATS_SUCCESS) {
            chats_ = UtilitySerializator::deserialize_chat_info_vec(connection_.in_stream_).second;
            emit get_chats_result(GET_CHATS_SUCCESS_ANSWER);
        } else if (answer_type == GET_CHATS_FAILED) {
            emit get_chats_result(GET_CHATS_FAILED_ANSWER);
        } else if (answer_type == FIND_CHAT_SUCCESS) {
            found_chat_info_.deserialize(connection_.in_stream_);
            emit find_chat_result(FIND_CHAT_SUCCESS_ANSWER);
        } else if (answer_type == FIND_CHAT_FAILED) {
            emit find_chat_result(FIND_CHAT_FAILED_ANSWER);
        } else if (answer_type == IS_CHAT_PARTICIPANT_SUCCESS) {
            bool is_participant = UtilitySerializator::deserialize_fundamental<bool>(connection_.in_stream_).second;
            emit is_chat_participant_result(IS_CHAT_PARTICIPANT_SUCCESS_ANSWER, is_participant);
        } else if (answer_type == IS_CHAT_PARTICIPANT_FAILED) {
            emit is_chat_participant_result(IS_CHAT_PARTICIPANT_FAILED_ANSWER, false);
        } else if (answer_type == ADD_PARTICIPANT_TO_CHAT_SUCCESS) {
            emit add_participant_to_chat_result(ADD_PARTICIPANT_TO_CHAT_SUCCESS_ANSWER);
        } else if (answer_type == ADD_PARTICIPANT_TO_CHAT_FAILED){
            emit add_participant_to_chat_result(ADD_PARTICIPANT_TO_CHAT_FAILED_ANSWER);
        } else if (answer_type == SEND_CHAT_MSG_SUCCESS) {
            emit send_msg_result(SEND_CHAT_MSG_SUCCESS_ANSWER);
        } else if (answer_type == SEND_CHAT_MSG_FAILED) {
            emit send_msg_result(SEND_CHAT_MSG_FAILED_ANSWER);
        } else if (answer_type == GET_CHAT_MSG_SUCCESS) {
            messages_ = UtilitySerializator::deserialize_msg_vec(connection_.in_stream_).second;
            emit get_chat_msg_result(GET_CHAT_MSG_SUCCESS_ANSWER);
        } else if (answer_type == GET_CHAT_MSG_FAILED) {
            emit get_chat_msg_result(GET_CHAT_MSG_FAILED_ANSWER);
        } else if (answer_type == DELETE_CHANNEL_SUCCESS) {
            emit delete_channel_result(DELETE_CHANNEL_SUCCESS_ANSWER);
        } else if (answer_type == DELETE_CHANNEL_FAILED) {
            emit delete_channel_result(DELETE_CHANNEL_FAILED_ANSWER);
        } else {
            std::cerr << "что-то невнятное: " << answer_type << '\n';
        }

        remove_delimeter();
    }
}

void Client::run_context() {
    std::thread t([&]() {
        if (io_context_ptr_->stopped()) {
            io_context_ptr_->restart();
        }

        io_context_ptr_->run();
    });

    t.detach();
}

void Client::remove_delimeter() {
    connection_.buffer_.consume(sizeof(END_OF_REQUEST));
}
