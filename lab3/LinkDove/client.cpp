#include "client.h"

#include <sstream>
#include <iostream>
#include <QDate>

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
                                  asio::placeholders::error(),
                                  asio::placeholders::bytes_transferred));

    run_context();
}

StatusInfo Client::get_status_info() {
    return status_info_;
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
    str_stream << COMPLAINT_REQUEST << '\n';

    complaint.serialize(str_stream);
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
        } else if (answer_type == COMPLAINT_SUCCESS) {
            emit complaint_result(SEND_COMPLAINT_SUCCESS_ANSWER);
        } else if (answer_type == COMPLAINT_FAILED){
            emit complaint_result(SEND_COMPLAINT_FAILED_ANSWER);
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
