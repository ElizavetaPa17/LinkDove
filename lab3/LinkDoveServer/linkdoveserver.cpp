#include "linkdoveserver.h"

#include <thread>
#include <mutex>
#include <iostream>
#include <boost/asio/ip/tcp.hpp>

#include <QSqlError>
#include <QDebug>

#include "UserInfo.h"
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
    std::cerr << request_type << '\n';

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
    }
}

void LinkDoveServer::handle_login_request(ConnectionIterator iterator) {
    LoginInfo login_info;
    login_info.deserialize(iterator->in_stream_);

    remove_delimeter(iterator);

    if (data_base_.login_user(login_info)) {
        iterator->out_stream_ << LOGIN_SUCCESS << "\n";

        // ДОБАВИТЬ TRY_CATCH (С ОТПРАВКОЙ КЛИЕНТУ INTERNAL_ERROR)
        StatusInfo status_info = data_base_.get_status_info(login_info.username_);
        status_info.serialize(iterator->out_stream_);

        iterator->out_stream_ << END_OF_REQUEST;
        async_write(iterator);
    } else {
        iterator->out_stream_ << LOGIN_FAILED << "\n" << END_OF_REQUEST;
        async_write(iterator);
    }
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
    unsigned long long complaint_id = Utility::deserialize_fundamental<unsigned long long>(iterator->in_stream_).second;

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
            Utility::serialize(answer, complaints);
            answer << END_OF_REQUEST;
        } catch (std::runtime_error& ex) {
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
    std::string username = Utility::deserialize_string(iterator->in_stream_).second;

    remove_delimeter(iterator);

    std::stringstream answer;
    try {
        StatusInfo status_info;
        status_info = data_base_.get_status_info(username);

        answer << FIND_USER_SUCCESS << "\n";
        status_info.serialize(answer);
        answer << END_OF_REQUEST;
        std::cerr << "find\n";
    } catch(std::runtime_error& ex) {
        answer << FIND_USER_FAILED << "\n" << END_OF_REQUEST;
        std::cerr << "not found\n";
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
