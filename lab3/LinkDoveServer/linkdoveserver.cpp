#include "linkdoveserver.h"

#include <thread>
#include <mutex>
#include <iostream>
#include <boost/asio/ip/tcp.hpp>

#include <QSqlError>
#include <QDebug>

#include "UserInfo.h"

std::mutex emplace_mutex;

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
    emplace_mutex.lock();
    connections_.emplace_back(io_context_ptr_);
    ConnectionIterator iterator = --connections_.end();
    emplace_mutex.unlock();

    acceptor_.async_accept(iterator->socket_,
                            boost::bind(&LinkDoveServer::handle_async_accept,
                                        shared_from_this(),
                                        iterator,
                                        boost::asio::placeholders::error()));
    run_context();
}

void LinkDoveServer::async_read(ConnectionIterator iterator) {
    asio::async_read_until(iterator->socket_,
                           iterator->buffer_,
                           END_OF_REQUEST,
                           boost::bind(&LinkDoveServer::handle_async_read,
                                       shared_from_this(),
                                       iterator,
                                       asio::placeholders::error(),
                                       asio::placeholders::bytes_transferred()));
}

void LinkDoveServer::handle_async_accept(ConnectionIterator iterator, boost::system::error_code error) {
    if (error) {
        std::cerr << "Failed to accept new connection: " << error.message();
        connections_.erase(iterator);
    } else {
        std::cout << "Connection from: " << iterator->socket_.remote_endpoint().address() << "\n";
    }

    async_read(iterator);
    start_async_accept();
}

void LinkDoveServer::handle_async_read(ConnectionIterator iterator, boost::system::error_code error, size_t bytes_transfered) {
    if (error) {
        std::cerr << "Failed to read from socket: " << error.value() << ' ' << error.message();
        connections_.erase(iterator);
    }

    if (bytes_transfered > 0) {
        handle_type_request(iterator);
    }
}

void LinkDoveServer::run_context() {
    std::thread t([&]() {
        io_context_ptr_->run();
    });

    t.detach();
}

void LinkDoveServer::handle_type_request(ConnectionIterator iterator) {
    std::string request_type;
    iterator->in_stream_ >> request_type;

    if (request_type == "LOGIN") {
        handle_login_request(iterator);
    } else if (request_type == "REGISTER") {
        handle_register_request(iterator);
    }
}

void LinkDoveServer::handle_login_request(ConnectionIterator iterator) {
    UserInfo user_info;
    user_info.deserialize(iterator->in_stream_);
}

void LinkDoveServer::handle_register_request(ConnectionIterator iterator) {
    UserInfo user_info;
    user_info.deserialize(iterator->in_stream_);

    if (data_base_.register_user(user_info)) {
        std::cerr << "Success user registration!\n";
    } else {
        std::cerr << "Failed to register the user\n";
    }
}
