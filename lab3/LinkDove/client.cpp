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
    setup_response_tree();
}

void Client::async_connect() {
    connection_.socket_.async_connect(endpoint_,
                                      boost::bind(&Client::handle_async_connect,
                                                  shared_from_this(),
                                                  asio::placeholders::error));

    run_context();
}

void Client::async_login(const LoginInfo& login_info) {
    connection_.out_stream_ << LOGIN_REQUEST << "\n";
    login_info.serialize(connection_.out_stream_);
    connection_.out_stream_ << END_OF_REQUEST;

    write_to_server();
}

void Client::async_register(const UserInfo& user_info) {
    connection_.out_stream_ << REGISTER_REQUEST << '\n';
    user_info.serialize(connection_.out_stream_);
    connection_.out_stream_ << END_OF_REQUEST;

    status_info_ = user_info.status_info_;

    write_to_server();
}

void Client::async_send_complaint(const std::string& text) {
    Complaint complaint;
    complaint.sender_id_ = status_info_.id_;
    complaint.text_ = text;

    connection_.out_stream_ << SEND_COMPLAINT_REQUEST << '\n';
    complaint.serialize(connection_.out_stream_);
    connection_.out_stream_ << END_OF_REQUEST;

    write_to_server();
}

// выделить отдельный stringstream, чтобы каждый раз не создавать на стеке новый
void Client::async_del_complaint(unsigned long long complaint_id) {
    connection_.out_stream_ << DEL_COMPLAINT_REQUEST << '\n';
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, complaint_id);
    connection_.out_stream_ << END_OF_REQUEST;

    write_to_server();
}

void Client::async_get_complaints() {
    connection_.out_stream_ << GET_COMPLAINTS_REQUEST << '\n' << END_OF_REQUEST;

    write_to_server();
}

void Client::async_update_user(StatusInfo& status_info) {
    // Идентификатор пользователя известен только классу Client
    // Поэтому status_info не является константным, т.к. нам необходимо установить поле идентификатора.
    updated_status_info_ = status_info;
    updated_status_info_.id_ = status_info_.id_;

    connection_.out_stream_ << UPDATE_USER_REQUEST << '\n';
    status_info.serialize(connection_.out_stream_);
    connection_.out_stream_ << END_OF_REQUEST;

    write_to_server();
}

void Client::async_find_user(const std::string &username) {
    connection_.out_stream_ << FIND_USER_REQUEST << '\n';
    UtilitySerializator::serialize(connection_.out_stream_, username);
    connection_.out_stream_ << END_OF_REQUEST;

    write_to_server();
}

void Client::async_ban_user(const std::string &username, bool is_ban) {
    connection_.out_stream_ << BAN_USER_REQUEST << '\n';
    UtilitySerializator::serialize(connection_.out_stream_, username);
    UtilitySerializator::serialize_fundamental<uint8_t>(connection_.out_stream_, is_ban);
    connection_.out_stream_ << END_OF_REQUEST;

    write_to_server();
}

void Client::async_ban_ind_user(const std::string &username, bool is_ban) {
    connection_.out_stream_ << BAN_IND_USER_REQUEST << '\n';
    UtilitySerializator::serialize(connection_.out_stream_, username);
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, status_info_.id_);
    UtilitySerializator::serialize_fundamental<uint8_t>(connection_.out_stream_, is_ban);
    connection_.out_stream_ << END_OF_REQUEST;

    write_to_server();
}

void Client::async_ban_chat_user(const std::string &username, unsigned long long chat_id, bool is_ban) {
    connection_.out_stream_ << BAN_CHAT_USER_REQUEST << '\n';
    UtilitySerializator::serialize(connection_.out_stream_, username);
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, chat_id);
    UtilitySerializator::serialize_fundamental<uint8_t>(connection_.out_stream_, is_ban);
    connection_.out_stream_ << END_OF_REQUEST;

    write_to_server();
}

void Client::async_is_banned_ind_user(const std::string &username) {
    connection_.out_stream_ << IS_BANNED_IND_USER_REQUEST << '\n';
    UtilitySerializator::serialize(connection_.out_stream_, username);
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, status_info_.id_);
    connection_.out_stream_ << END_OF_REQUEST;

    write_to_server();
}

void Client::async_is_banned_chat_user(unsigned long long chat_id) {
    connection_.out_stream_ << IS_BANNED_CHAT_USER_REQUEST << '\n';
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, status_info_.id_);
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, chat_id);
    connection_.out_stream_ << END_OF_REQUEST;

    write_to_server();
}

void Client::async_get_banned_interlocutors() {
    connection_.out_stream_ << GET_BANNED_INTERLOCUTORS_REQUEST << '\n';
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, status_info_.id_);
    connection_.out_stream_ << END_OF_REQUEST;

    write_to_server();
}

void Client::async_send_message(const IMessage& message) {
    connection_.out_stream_ << SEND_MSG_REQUEST << '\n';
    UtilitySerializator::serialize(connection_.out_stream_, message);
    connection_.out_stream_ << END_OF_REQUEST;

    write_to_server();
}

void Client::async_get_ind_messages(unsigned long long other_id) {
    connection_.out_stream_ << GET_IND_MSG_REQUEST << '\n';
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, status_info_.id_);
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, other_id);
    connection_.out_stream_ << END_OF_REQUEST;

    write_to_server();
}

void Client::async_get_interlocutors() {
    connection_.out_stream_ << GET_INTERLOCUTORS_REQUEST << '\n';
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, status_info_.id_);
    connection_.out_stream_ << END_OF_REQUEST;

    write_to_server();
}

void Client::async_delete_ind_chat(unsigned long long interlocutor_id) {
    connection_.out_stream_ << DELETE_IND_CHAT_REQUEST  << '\n';
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, status_info_.id_);
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, interlocutor_id);
    connection_.out_stream_ << END_OF_REQUEST;

    write_to_server();
}

void Client::async_create_channel(const std::pair<std::string, bool> &info) {
    connection_.out_stream_ << CREATE_CHANNEL_REQUEST << '\n';

    ChannelInfo channel_info;
    channel_info.name_ = info.first;
    channel_info.owner_id_ = status_info_.id_;
    channel_info.is_private_ = info.second;
    channel_info.serialize(connection_.out_stream_);
    connection_.out_stream_ << END_OF_REQUEST;

    write_to_server();
}

void Client::async_find_channel(const std::string &channel_name) {
    connection_.out_stream_ << FIND_CHANNEL_REQUEST << '\n';
    UtilitySerializator::serialize(connection_.out_stream_, channel_name);
    connection_.out_stream_ << END_OF_REQUEST;

    write_to_server();
}

void Client::async_get_channels() {
    connection_.out_stream_ << GET_CHANNELS_REQUEST << '\n';
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, status_info_.id_);
    connection_.out_stream_ << END_OF_REQUEST;


    write_to_server();
}

void Client::async_is_channel_participant_request(unsigned long long channel_id) {
    connection_.out_stream_ << IS_CHANNEL_PARTICIPANT_REQUEST << '\n';
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, status_info_.id_);
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, channel_id);
    connection_.out_stream_ << END_OF_REQUEST;

    write_to_server();
}

void Client::async_add_channel_participant_request(unsigned long long channel_id) {
    connection_.out_stream_ << ADD_PARTICIPANT_TO_CHANNEL_REQUEST << '\n';
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, status_info_.id_);
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, channel_id);
    connection_.out_stream_ << END_OF_REQUEST;

    write_to_server();
}

void Client::async_add_private_channel_participant_request(const std::string &username, unsigned long long channel_id) {
    connection_.out_stream_ << ADD_PARTICIPANT_TO_PRIVATE_CHANNEL_REQUEST << '\n';
    UtilitySerializator::serialize(connection_.out_stream_, username);
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, channel_id);
    connection_.out_stream_ << END_OF_REQUEST;

    write_to_server();
}

void Client::async_add_private_chat_participant_request(const std::string &username, unsigned long long chat_id) {
    connection_.out_stream_ << ADD_PARTICIPANT_TO_PRIVATE_CHAT_REQUEST << '\n';
    UtilitySerializator::serialize(connection_.out_stream_, username);
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, chat_id);
    connection_.out_stream_ << END_OF_REQUEST;

    write_to_server();
}

void Client::async_request_channel_participant_request(unsigned long long channel_id) {
    connection_.out_stream_ << REQUEST_PARTICIPANT_TO_CHANNEL_REQUEST << '\n';
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, status_info_.id_);
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, channel_id);
    connection_.out_stream_ << END_OF_REQUEST;

    write_to_server();
}

void Client::async_delete_request_channel_request(const std::string &username, unsigned long long channel_id) {
    connection_.out_stream_ << REMOVE_REQUEST_CHANNEL_REQUEST << '\n';
    UtilitySerializator::serialize(connection_.out_stream_, username);
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, channel_id);
    connection_.out_stream_ << END_OF_REQUEST;

    write_to_server();
}

void Client::async_request_chat_participant_request(unsigned long long chat_id) {
    connection_.out_stream_ << REQUEST_PARTICIPANT_TO_CHAT_REQUEST << '\n';
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, status_info_.id_);
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, chat_id);
    connection_.out_stream_ << END_OF_REQUEST;

    write_to_server();
}

void Client::async_delete_request_chat_request(const std::string &username, unsigned long long chat_id) {
    connection_.out_stream_ << REMOVE_REQUEST_CHAT_REQUEST << '\n';
    UtilitySerializator::serialize(connection_.out_stream_, username);
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, chat_id);
    connection_.out_stream_ << END_OF_REQUEST;

    write_to_server();
}

void Client::async_get_channel_messages(unsigned long long channel_id) {
    connection_.out_stream_ << GET_CHNNL_MSG_REQUEST << '\n';
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, channel_id);
    connection_.out_stream_ << END_OF_REQUEST;

    write_to_server();
}

void Client::async_get_channel_participants(unsigned long long channel_id) {
    connection_.out_stream_ << GET_CHNNL_PARTICIPANTS_REQUEST << '\n';
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, channel_id);
    connection_.out_stream_ << END_OF_REQUEST;

    write_to_server();
}

void Client::async_get_channel_requests_request(unsigned long long channel_id) {
    connection_.out_stream_ << GET_CHNNL_REQUESTS_REQUEST << '\n';
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, channel_id);
    connection_.out_stream_ << END_OF_REQUEST;

    write_to_server();
}

void Client::async_delete_channel(unsigned long long channel_id) {
    connection_.out_stream_ << DELETE_CHANNEL_REQUEST << '\n';
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, channel_id);
    connection_.out_stream_ << END_OF_REQUEST;

    write_to_server();
}

void Client::async_remove_user_from_channel(unsigned long long channel_id, const std::string &user_name) {
    connection_.out_stream_ << REMOVE_USER_FROM_CHANNEL_REQUEST << '\n';
    UtilitySerializator::serialize(connection_.out_stream_, user_name);
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, channel_id);
    connection_.out_stream_ << END_OF_REQUEST;

    write_to_server();
}

void Client::async_quit_channel(unsigned long long user_id, unsigned long long channel_id) {
    connection_.out_stream_ << QUIT_CHANNEL_REQUEST << '\n';
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, user_id);
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, channel_id);
    connection_.out_stream_ << END_OF_REQUEST;

    write_to_server();
}

void Client::async_create_chat(const std::pair<std::string, bool> &info) {
    connection_.out_stream_ << CREATE_CHAT_REQUEST << '\n';

    ChatInfo chat_info;
    chat_info.name_ = info.first;
    chat_info.owner_id_ = status_info_.id_;
    chat_info.is_private_ = info.second;
    chat_info.serialize(connection_.out_stream_);
    connection_.out_stream_ << END_OF_REQUEST;

    write_to_server();
}

void Client::async_get_chats() {
    connection_.out_stream_ << GET_CHATS_REQUEST << '\n';
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, status_info_.id_);
    connection_.out_stream_ << END_OF_REQUEST;

    write_to_server();
}

void Client::async_find_chat(const std::string &chat_name) {
    connection_.out_stream_ << FIND_CHAT_REQUEST << "\n";
    UtilitySerializator::serialize(connection_.out_stream_, chat_name);
    connection_.out_stream_ << END_OF_REQUEST;

    write_to_server();
}

void Client::async_is_chat_participant_request(unsigned long long chat_id) {
    connection_.out_stream_ << IS_CHAT_PARTICIPANT_REQUEST << '\n';
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, status_info_.id_);
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, chat_id);
    connection_.out_stream_ << END_OF_REQUEST;

    write_to_server();
}

void Client::async_add_chat_participant_request(unsigned long long chat_id) {
    connection_.out_stream_ << ADD_PARTICIPANT_TO_CHAT_REQUEST << '\n';
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, status_info_.id_);
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, chat_id);
    connection_.out_stream_ << END_OF_REQUEST;

    write_to_server();
}

void Client::async_get_chat_messages(unsigned long long chat_id) {
    connection_.out_stream_ << GET_CHAT_MSG_REQUEST << '\n';
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, chat_id);
    connection_.out_stream_ << END_OF_REQUEST;

    write_to_server();
}

void Client::async_get_chat_participants(unsigned long long group_id) {
    connection_.out_stream_ << GET_CHAT_PARTICIPANTS_REQUEST << '\n';
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, group_id);
    connection_.out_stream_ << END_OF_REQUEST;

    write_to_server();
}

void Client::async_get_chat_requests_request(unsigned long long chat_id) {
    connection_.out_stream_ << GET_CHAT_REQUESTS_REQUEST << '\n';
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, chat_id);
    connection_.out_stream_ << END_OF_REQUEST;

    write_to_server();
}

void Client::async_delete_chat(unsigned long long group_id) {
    connection_.out_stream_ << DELETE_CHAT_REQUEST << '\n';
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, group_id);
    connection_.out_stream_ << END_OF_REQUEST;

    write_to_server();
}

void Client::async_quit_chat(unsigned long long user_id, unsigned long long group_id) {
    connection_.out_stream_ << QUIT_CHAT_REQUEST << '\n';
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, user_id);
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, group_id);

    connection_.out_stream_ << END_OF_REQUEST;

    write_to_server();
}

void Client::async_remove_user_from_chat(unsigned long long group_id, const std::string &user_name) {
    connection_.out_stream_ << REMOVE_USER_FROM_CHAT_REQUEST << 'n';
    UtilitySerializator::serialize(connection_.out_stream_, user_name);
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, group_id);
    connection_.out_stream_ << END_OF_REQUEST;

    write_to_server();
}

void Client::async_answer_user(unsigned long long user_id, const std::string &text) {
    connection_.out_stream_ << SEND_USER_ANSWER_REQUEST << '\n';
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, user_id);
    UtilitySerializator::serialize(connection_.out_stream_, text);
    connection_.out_stream_ << END_OF_REQUEST;

    write_to_server();
}

void Client::async_get_notifications() {
    connection_.out_stream_ << GET_NOTIFICATIONS_REQUEST << '\n';
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, status_info_.id_);

    connection_.out_stream_ << END_OF_REQUEST;
    write_to_server();
}

void Client::async_del_notification(unsigned long long id) {
    connection_.out_stream_ << DEL_NOTIFICATION_REQUEST << '\n';
    UtilitySerializator::serialize_fundamental<unsigned long long>(connection_.out_stream_, id);
    connection_.out_stream_ << END_OF_REQUEST;

    write_to_server();
}

void Client::async_delete_msg(const IMessage &msg) {
    connection_.out_stream_ << DEL_MSG_REQUEST << '\n';
    UtilitySerializator::serialize(connection_.out_stream_, msg);
    connection_.out_stream_ << END_OF_REQUEST;

    write_to_server();
}

StatusInfo Client::get_status_info() {
    return status_info_;
}

StatusInfo Client::get_found_user() {
    return found_status_info_;
}

ChannelInfo Client::get_found_channel() {
    return found_channel_info_;
}

ChatInfo Client::get_found_chat() {
    return found_chat_info_;
}

bool Client::is_connected() noexcept {
    return is_connected_;
}


void Client::setup_response_tree() {
    response_tree_[LOGIN_SUCCESS] = [this]() { status_info_.deserialize(connection_.in_stream_);
                                               emit authorization_result(LOGIN_SUCCESS_ANSWER); };
    response_tree_[LOGIN_BANNED] = [this] () { emit authorization_result(LOGIN_BANNED_ANSWER); };
    response_tree_[LOGIN_FAILED] = [this] () { emit authorization_result(LOGIN_FAILED_ANSWER); };

    response_tree_[REGISTER_SUCCESS] = [this] () { status_info_.deserialize(connection_.in_stream_);
                                                   emit authorization_result(REGISTRATION_SUCCESS_ANSWER); };
    response_tree_[REGISTER_FAILED]  = [this] () { emit authorization_result(REGISTRATION_FAILED_ANSWER); };

    response_tree_[SEND_COMPLAINT_SUCCESS] = [this] () { emit send_complaint_result(SEND_COMPLAINT_SUCCESS_ANSWER); };
    response_tree_[SEND_COMPLAINT_FAILED]  = [this] () { emit send_complaint_result(SEND_COMPLAINT_FAILED_ANSWER); };

    response_tree_[DEL_COMPLAINT_SUCCESS] = [this] () { emit del_complaint_result(DEL_COMPLAINT_SUCCESS_ANSWER); };
    response_tree_[DEL_COMPLAINT_FAILED]  = [this] () { emit del_complaint_result(DEL_COMPLAINT_FAILED_ANSWER); };

    response_tree_[GET_COMPLAINTS_SUCCESS] = [this] () { std::vector<Complaint> complaints = UtilitySerializator::deserialize_compl_vec(connection_.in_stream_).second;
                                                         emit get_complaints_result(GET_COMPLAINTS_SUCCESS_ANSWER, complaints); };
    response_tree_[GET_COMPLAINTS_FAILED]  = [this] () { emit get_complaints_result(GET_COMPLAINTS_FAILED_ANSWER, std::vector<Complaint>()); };

    response_tree_[UPDATE_USER_SUCCESS] = [this] () { status_info_ = updated_status_info_;
                                                      emit update_user_result(UPDATE_USER_SUCCESS_ANSWER); };
    response_tree_[UPDATE_USER_FAILED]  = [this] () { emit update_user_result(UPDATE_USER_FAILED_ANSWER); };

    response_tree_[FIND_USER_SUCCESS] = [this] () { found_status_info_.deserialize(connection_.in_stream_);
                                                    emit find_user_result(FIND_USER_SUCCESS_ANSWER); };
    response_tree_[FIND_USER_FAILED]  = [this] () { emit find_user_result(FIND_USER_FAILED_ANWSER); };

    response_tree_[BAN_USER_SUCCESS] = [this] () { emit ban_user_result(BAN_USER_SUCCESS_ANSWER); };
    response_tree_[BAN_USER_FAILED]  = [this] () { emit ban_user_result(BAN_USER_FAILED_ANSWER); };

    response_tree_[BAN_IND_USER_SUCCESS] = [this] () { emit ban_user_result(BAN_IND_USER_SUCCESS_ANSWER); };
    response_tree_[BAN_IND_USER_FAILED]  = [this] () { emit ban_user_result(BAN_IND_USER_FAILED_ANSWER); };

    response_tree_[BAN_CHAT_USER_SUCCESS] = [this] () { emit ban_user_result(BAN_CHAT_USER_SUCCESS_ANSWER); };
    response_tree_[BAN_CHAT_USER_FAILED]  = [this] () { emit ban_user_result(BAN_CHAT_USER_FAILED_ANSWER); };

    response_tree_[IS_BANNED_IND_USER_SUCCESS] = [this] () { bool is_banned = UtilitySerializator::deserialize_fundamental<bool>(connection_.in_stream_).second;
                                                             emit is_banned_user_result(IS_IND_BANNED_USER_SUCCESS_ANSWER, is_banned); };
    response_tree_[IS_BANNED_IND_USER_FAILED]  = [this] () { emit is_banned_user_result(IS_IND_BANNED_USER_FAILED_ANSWER, true); };

    response_tree_[IS_BANNED_CHAT_USER_SUCCESS] = [this] () { bool is_banned = UtilitySerializator::deserialize_fundamental<bool>(connection_.in_stream_).second;
                                                              emit is_banned_user_result(IS_CHAT_BANNED_USER_SUCCESS_ANSWER, is_banned);
                                                            };
    response_tree_[IS_BANNED_CHAT_USER_FAILED]  = [this] () { emit is_banned_user_result(IS_CHAT_BANNED_USER_FAILED_ANSWER, true); };

    response_tree_[GET_BANNED_INTERLOCUTORS_SUCCESS] = [this] () { std::vector<std::string> interlocutors = UtilitySerializator::deserialize_vec_string(connection_.in_stream_).second;
                                                                   emit get_banned_users(GET_BANNED_INTERLOCUTORS_SUCCESS_ANSWER, interlocutors); };
    response_tree_[GET_BANNED_INTERLOCUTORS_FAILED]  = [this] () { emit get_banned_users(GET_BANNED_INTERLOCUTORS_SUCCESS_ANSWER, std::vector<std::string>()); };

    response_tree_[SEND_IND_MSG_SUCCESS] = [this] () { emit send_msg_result(SEND_IND_MSG_SUCCESS_ANSWER); };
    response_tree_[SEND_IND_MSG_FAILED]  = [this] () { emit send_msg_result(SEND_IND_MSG_FAILED_ANSWER); };

    response_tree_[GET_IND_MSG_SUCCESS] = [this] () { std::vector<std::shared_ptr<IMessage>> messages = UtilitySerializator::deserialize_msg_vec(connection_.in_stream_).second;
                                                      emit get_ind_msg_result(GET_IND_MSG_SUCCESS_ANSWER, messages); };
    response_tree_[GET_IND_MSG_FAILED]  = [this] () { emit get_ind_msg_result(GET_IND_MSG_FAILED_ANSWER, std::vector<std::shared_ptr<IMessage>>()); };

    response_tree_[GET_INTERLOCUTORS_SUCCESS] = [this] () { std::vector<StatusInfo> status_info_vec = UtilitySerializator::deserialize_st_info_vec(connection_.in_stream_).second;
                                                            emit get_interlocutors_result(GET_INTERLOCUTORS_SUCCESS_ANSWER, status_info_vec); };
    response_tree_[GET_INTERLOCUTORS_FAILED]  = [this] () { emit get_interlocutors_result(GET_INTERLOCUTORS_FAILED_ANSWER, std::vector<StatusInfo>());};

    response_tree_[CREATE_CHANNEL_SUCCESS] = [this] () { emit create_channel_result(CREATE_CHANNEL_SUCCESS_ANSWER); };
    response_tree_[CREATE_CHANNEL_FAILED]  = [this] () { emit create_channel_result(CREATE_CHANNEL_FAILED_ANSWER);};

    response_tree_[FIND_CHANNEL_SUCCESS] = [this] () { found_channel_info_.deserialize(connection_.in_stream_);
                                                       emit find_channel_result(FIND_CHANNEL_SUCCESS_ANSWER); };
    response_tree_[FIND_CHANNEL_FAILED]  = [this] () { emit find_channel_result(FIND_CHANNEL_FAILED_ANSWER); };

    response_tree_[GET_CHANNELS_SUCCESS] = [this] () { std::vector<ChannelInfo> channels = UtilitySerializator::deserialize_ch_info_vec(connection_.in_stream_).second;
                                                       emit get_channels_result(GET_CHANNELS_SUCCESS_ANSWER, channels); };
    response_tree_[GET_CHANNELS_FAILED]  = [this] () { emit get_channels_result(GET_CHANNELS_FAILED_ANSWER, std::vector<ChannelInfo>()); };

    response_tree_[IS_CHANNEL_PARTICIPANT_SUCCESS] = [this] () { bool is_participant = UtilitySerializator::deserialize_fundamental<bool>(connection_.in_stream_).second;
                                                                 emit is_channel_participant_result(IS_CHANNEL_PARTICIPANT_SUCCESS_ANSWER, is_participant); };
    response_tree_[IS_CHANNEL_PARTICIPANT_FAILED]  = [this] () { emit is_channel_participant_result(IS_CHANNEL_PARTICIPANT_FAILED_ANSWER, false); };

    response_tree_[ADD_PARTICIPANT_TO_CHANNEL_SUCCESS] = [this] () { emit add_participant_to_channel_result(ADD_PARTICIPANT_TO_CHANNEL_SUCCESS_ANSWER); };
    response_tree_[ADD_PARTICIPANT_TO_CHANNEL_FAILED]  = [this] () { emit add_participant_to_channel_result(ADD_PARTICIPANT_TO_CHANNEL_FAILED_ANSWER); };

    response_tree_[REQUEST_PARTICIPANT_TO_CHANNEL_SUCCESS] = [this] () { emit request_participant_to_channel_result(REQUEST_PARTICIPANT_TO_CHANNEL_SUCCESS_ANSWER); };
    response_tree_[REQUEST_PARTICIPANT_TO_CHANNEL_FAILED]  = [this] () { emit request_participant_to_channel_result(REQUEST_PARTICIPANT_TO_CHANNEL_FAILED_ANSWER); };

    response_tree_[REQUEST_PARTICIPANT_TO_CHAT_SUCCESS] = [this] () { emit request_participant_to_chat_result(REQUEST_PARTICIPANT_TO_CHAT_SUCCESS_ANSWER); };
    response_tree_[REQUEST_PARTICIPANT_TO_CHAT_FAILED]  = [this] () { emit request_participant_to_chat_result(REQUEST_PARTICIPANT_TO_CHAT_FAILED_ANSWER); };

    response_tree_[SEND_CHNNL_MSG_SUCCESS] = [this] () { emit send_msg_result(SEND_CHNNL_MSG_SUCCESS_ANSWER); };
    response_tree_[SEND_CHNNL_MSG_FAILED]  = [this] () { emit send_msg_result(SEND_CHNNL_MSG_FAILED_ANSWER); };

    response_tree_[GET_CHNNL_MSG_SUCCESS] = [this] () { std::vector<std::shared_ptr<IMessage>> messages = UtilitySerializator::deserialize_msg_vec(connection_.in_stream_).second;
                                                        emit get_channel_msg_result(GET_CHNNL_MSG_SUCCESS_ANSWER, messages); };
    response_tree_[GET_CHNNL_MSG_FAILED]  = [this] () { emit get_channel_msg_result(GET_CHNNL_MSG_FAILED_ANSWER, std::vector<std::shared_ptr<IMessage>>()); };

    response_tree_[CREATE_CHAT_SUCCESS] = [this] () { emit create_chat_result(CREATE_CHAT_SUCCESS_ANSWER); };
    response_tree_[CREATE_CHAT_FAILED]  = [this] () { emit create_chat_result(CREATE_CHAT_FAILED_ANSWER); };

    response_tree_[GET_CHATS_SUCCESS] = [this] () { std::vector<ChatInfo> chats = UtilitySerializator::deserialize_chat_info_vec(connection_.in_stream_).second;
                                                    emit get_chats_result(GET_CHATS_SUCCESS_ANSWER, chats); };
    response_tree_[GET_CHATS_FAILED]  = [this] () { emit get_chats_result(GET_CHATS_FAILED_ANSWER, std::vector<ChatInfo>()); };

    response_tree_[FIND_CHAT_SUCCESS] = [this] () { found_chat_info_.deserialize(connection_.in_stream_);
                                                    emit find_chat_result(FIND_CHAT_SUCCESS_ANSWER);};
    response_tree_[FIND_CHAT_FAILED]  = [this] () { emit find_chat_result(FIND_CHAT_FAILED_ANSWER); };

    response_tree_[IS_CHAT_PARTICIPANT_SUCCESS] = [this] () { bool is_participant = UtilitySerializator::deserialize_fundamental<bool>(connection_.in_stream_).second;
                                                              emit is_chat_participant_result(IS_CHAT_PARTICIPANT_SUCCESS_ANSWER, is_participant); };
    response_tree_[IS_CHAT_PARTICIPANT_FAILED]  = [this] () { emit is_chat_participant_result(IS_CHAT_PARTICIPANT_FAILED_ANSWER, false); };

    response_tree_[ADD_PARTICIPANT_TO_CHAT_SUCCESS] = [this] () { emit add_participant_to_chat_result(ADD_PARTICIPANT_TO_CHAT_SUCCESS_ANSWER); };
    response_tree_[ADD_PARTICIPANT_TO_CHAT_FAILED]  = [this] () { emit add_participant_to_chat_result(ADD_PARTICIPANT_TO_CHAT_FAILED_ANSWER); };

    response_tree_[SEND_CHAT_MSG_SUCCESS] = [this] () { emit send_msg_result(SEND_CHAT_MSG_SUCCESS_ANSWER); };
    response_tree_[SEND_CHAT_MSG_FAILED]  = [this] () { emit send_msg_result(SEND_CHAT_MSG_FAILED_ANSWER); };

    response_tree_[GET_CHAT_MSG_SUCCESS] = [this] () { std::vector<std::shared_ptr<IMessage>> messages = UtilitySerializator::deserialize_msg_vec(connection_.in_stream_).second;
                                                       emit get_chat_msg_result(GET_CHAT_MSG_SUCCESS_ANSWER, messages); };
    response_tree_[GET_CHAT_MSG_FAILED]  = [this] () { emit get_chat_msg_result(GET_CHAT_MSG_FAILED_ANSWER, std::vector<std::shared_ptr<IMessage>>()); };

    response_tree_[DELETE_CHANNEL_SUCCESS] = [this] () { emit delete_channel_result(DELETE_CHANNEL_SUCCESS_ANSWER); };
    response_tree_[DELETE_CHANNEL_FAILED]  = [this] () { emit delete_channel_result(DELETE_CHANNEL_FAILED_ANSWER); };

    response_tree_[DELETE_CHAT_SUCCESS] = [this] () { emit delete_chat_result(DELETE_CHAT_SUCCESS_ANSWER); };
    response_tree_[DELETE_CHAT_FAILED]  = [this] () { emit delete_chat_result(DELETE_CHAT_FAILED_ANSWER); };

    response_tree_[DELETE_IND_CHAT_SUCCESS] = [this] () { emit delete_ind_chat_result(DELETE_IND_CHAT_SUCCESS_ANSWER); };
    response_tree_[DELETE_IND_CHAT_FAILED]  = [this] () { emit delete_ind_chat_result(DELETE_IND_CHAT_FAILED_ANSWER); };

    response_tree_[QUIT_CHAT_SUCCESS] = [this] () { emit quit_chat_result(QUIT_CHAT_SUCCESS_ANSWER); };
    response_tree_[QUIT_CHAT_FAILED]  = [this] () { emit quit_chat_result(QUIT_CHAT_FAILED_ANSWER); };

    response_tree_[QUIT_CHANNEL_SUCCESS] = [this] () { emit quit_channel_result(QUIT_CHANNEL_SUCCESS_ANSWER); };
    response_tree_[QUIT_CHANNEL_FAILED]  = [this] () { emit quit_channel_result(QUIT_CHANNEL_FAILED_ANSWER); };

    response_tree_[REMOVE_USER_FROM_CHANNEL_SUCCESS] = [this] () { emit remove_user_from_channel_result(REMOVE_USER_FROM_CHANNEL_SUCCESS_ANSWER); };
    response_tree_[REMOVE_USER_FROM_CHANNEL_FAILED]  = [this] () { emit remove_user_from_channel_result(REMOVE_USER_FROM_CHANNEL_FAILED_ANSWER); };

    response_tree_[REMOVE_USER_FROM_CHAT_SUCCESS] = [this] () { emit remove_user_from_chat_result(REMOVE_USER_FROM_CHAT_SUCCESS_ANSWER); };
    response_tree_[REMOVE_USER_FROM_CHAT_FAILED]  = [this] () { emit remove_user_from_chat_result(REMOVE_USER_FROM_CHAT_FAILED_ANSWER); };

    response_tree_[GET_CHNNL_PARTICIPANTS_SUCCESS] = [this] () { std::vector<std::string> participants = UtilitySerializator::deserialize_vec_string(connection_.in_stream_).second;
                                                                 emit get_channel_participants_result(GET_CHNNL_PARTICIPANTS_SUCCESS_ANSWER, participants); };
    response_tree_[GET_CHNNL_PARTICIPANTS_FAILED]  = [this] () { emit get_channel_participants_result(GET_CHNNL_PARTICIPANTS_FAILED_ANSWER, std::vector<std::string>()); };

    response_tree_[GET_CHNNL_REQUESTS_SUCCESS] = [this] () { std::vector<std::string> participants = UtilitySerializator::deserialize_vec_string(connection_.in_stream_).second;
                                                             emit get_channel_requests_result(GET_CHNNL_PARTICIPANTS_SUCCESS_ANSWER, participants); };
    response_tree_[GET_CHNNL_REQUESTS_FAILED]  = [this] () { emit get_channel_requests_result(GET_CHNNL_PARTICIPANTS_FAILED_ANSWER, std::vector<std::string>()); };

    response_tree_[GET_CHAT_REQUESTS_SUCCESS] = [this] () { std::vector<std::string> participants = UtilitySerializator::deserialize_vec_string(connection_.in_stream_).second;
                                                             emit get_chat_requests_result(GET_CHAT_PARTICIPANTS_SUCCESS_ANSWER, participants); };
    response_tree_[GET_CHAT_REQUESTS_FAILED]  = [this] () { emit get_chat_requests_result(GET_CHAT_PARTICIPANTS_FAILED_ANSWER, std::vector<std::string>()); };

    response_tree_[GET_CHAT_PARTICIPANTS_SUCCESS] = [this] () { std::vector<std::string> participants = UtilitySerializator::deserialize_vec_string(connection_.in_stream_).second;
                                                                emit get_chat_participants_result(GET_CHAT_PARTICIPANTS_SUCCESS_ANSWER, participants); };
    response_tree_[GET_CHAT_PARTICIPANTS_FAILED]  = [this] () { emit get_chat_participants_result(GET_CHAT_PARTICIPANTS_FAILED_ANSWER, std::vector<std::string>()); };

    response_tree_[SEND_USER_ANSWER_SUCCESS] = [this] () { emit answer_user_result(SEND_USER_ANSWER_SUCCESS_ANSWER); };
    response_tree_[SEND_USER_ANSWER_FAILED]  = [this] () { emit answer_user_result(SEND_USER_ANSWER_FAILED_ANSWER); };

    response_tree_[GET_NOTIFICATIONS_SUCCESS] = [this] () { std::vector<Notification> notifications = UtilitySerializator::deserialize_not_vec(connection_.in_stream_).second;
                                                            emit get_notifications_result(GET_NOTIFICATIONS_SUCCESS_ANSWER, notifications); };
    response_tree_[GET_NOTIFICATIONS_FAILED]  = [this] () { emit get_notifications_result(GET_NOTIFICATIONS_FAILED_ANSWER, std::vector<Notification>()); };

    response_tree_[DEL_NOTIFICATION_SUCCESS] = [this] () { emit del_notification_result(DEL_NOTIFICATION_SUCCESS_ANSWER); };
    response_tree_[DEL_NOTIFICATION_FAILED]  = [this] () { emit del_notification_result(DEL_NOTIFICATION_FAILED_ANSWER); };

    response_tree_[DEL_IND_MSG_SUCCESS] = [this] () { emit delete_msg_result(DEL_IND_MSG_SUCCESS_ANSWER); };
    response_tree_[DEL_IND_MSG_FAILED]  = [this] () { emit delete_msg_result(DEL_IND_MSG_FAILED_ANSWER); };

    response_tree_[DEL_CHANNEL_MSG_SUCCESS] = [this] () { emit delete_msg_result(DEL_CHANNEL_MSG_SUCCESS_ANSWER); };
    response_tree_[DEL_CHANNEL_MSG_FAILED]  = [this] () { emit delete_msg_result(DEL_CHANNEL_MSG_FAILED_ANSWER); };

    response_tree_[DEL_CHAT_MSG_SUCCESS] = [this] () { emit delete_msg_result(DEL_CHAT_MSG_SUCCESS_ANSWER); };
    response_tree_[DEL_CHAT_MSG_FAILED]  = [this] () { emit delete_msg_result(DEL_CHAT_MSG_FAILED_ANSWER); };

    response_tree_[ADD_PARTICIPANT_TO_PRIVATE_CHANNEL_SUCCESS] = [this] () { emit add_private_channel_participant_result(ADD_PARTICIPANT_TO_PRIVATE_CHANNEL_SUCCESS_ANSWER); };
    response_tree_[ADD_PARTICIPANT_TO_PRIVATE_CHANNEL_FAILED]  = [this] () { emit add_private_channel_participant_result(ADD_PARTICIPANT_TO_PRIVATE_CHANNEL_FAILED_ANSWER); };

    response_tree_[REMOVE_REQUEST_CHANNEL_SUCCESS] = [this] () { emit remove_request_channel_result(REMOVE_REQUEST_CHANNEL_SUCCESS_ANSWER); };
    response_tree_[REMOVE_REQUEST_CHANNEL_FAILED]  = [this] () { emit remove_request_channel_result(REMOVE_REQUEST_CHANNEL_FAILED_ANSWER); };

    response_tree_[ADD_PARTICIPANT_TO_PRIVATE_CHAT_SUCCESS] = [this] () { emit add_private_chat_participant_result(ADD_PARTICIPANT_TO_PRIVATE_CHAT_SUCCESS_ANSWER); };
    response_tree_[ADD_PARTICIPANT_TO_PRIVATE_CHAT_FAILED]  = [this] () { emit add_private_chat_participant_result(ADD_PARTICIPANT_TO_PRIVATE_CHAT_FAILED_ANSWER); };

    response_tree_[REMOVE_REQUEST_CHAT_SUCCESS] = [this] () { emit remove_request_chat_result(REMOVE_REQUEST_CHAT_SUCCESS_ANSWER); };
    response_tree_[REMOVE_REQUEST_CHAT_FAILED]  = [this] () { emit remove_request_chat_result(REMOVE_REQUEST_CHAT_FAILED_ANSWER); };
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
        std::cerr << answer_type << '\n';

        response_tree_[answer_type]();
        remove_delimeter();
    }
}

void Client::write_to_server() {
    std::stringstream buf;
    std::cerr << "SEND: \n" << buf.str() << '\n';

    asio::async_write(connection_.socket_, connection_.buffer_,
                       boost::bind(&Client::handle_async_write,
                                   shared_from_this(),
                                   asio::placeholders::error,
                                   asio::placeholders::bytes_transferred));

    run_context();
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
