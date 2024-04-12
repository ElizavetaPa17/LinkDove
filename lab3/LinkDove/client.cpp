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

void Client::async_del_complaint(unsigned long long complaint_id) {
    connection_.out_stream_ << DEL_COMPLAINT_REQUEST << "\n";
    Utility::serialize_fundamental<unsigned long long>(connection_.out_stream_, complaint_id);
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
    Utility::serialize(connection_.out_stream_, username);
    connection_.out_stream_ << END_OF_REQUEST;

    asio::async_write(connection_.socket_, connection_.buffer_,
                      boost::bind(&Client::handle_async_write,
                                  shared_from_this(),
                                  asio::placeholders::error,
                                  asio::placeholders::bytes_transferred));

    run_context();
}

StatusInfo Client::get_status_info() {
    return status_info_;
}

std::vector<Complaint> Client::get_complaints() {
    return complaints_;
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
    Utility::serialize_fundamental(str_stream, complaint_id);
    str_stream << END_OF_REQUEST;

    return str_stream.str();
}

std::string Client::create_update_user_request(const StatusInfo& status_info) {
    std::stringstream str_stream;
    str_stream << UPDATE_USER_REQUEST << '\n';

    status_info.serialize(str_stream);
    str_stream << END_OF_REQUEST;

    std::cerr << str_stream.str() << "\n\n";
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

        if (answer_type == LOGIN_SUCCESS) {
            status_info_.deserialize(connection_.in_stream_);
            emit authorization_result(LOGIN_SUCCESS_ANSWER);
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
            complaints_ = Utility::deserialize_compl_vec(connection_.in_stream_).second;
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
