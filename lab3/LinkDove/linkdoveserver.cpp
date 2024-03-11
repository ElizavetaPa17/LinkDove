#include "linkdoveserver.h"

#include <thread>
#include <iostream>
#include <boost/asio/ip/tcp.hpp>

LinkDoveServer::LinkDoveServer()
    : io_context_ptr_(std::make_shared<asio::io_context>())
    , acceptor_(*io_context_ptr_)
{

}

void LinkDoveServer::listen(uint16_t port) {
    auto endpoint = tcp::endpoint(tcp::v4(), port);

    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(tcp::acceptor::reuse_address(true));
    acceptor_.bind(endpoint);
    acceptor_.listen();

    start_accept();
}

void LinkDoveServer::start_accept() {
    connections_.emplace_back(io_context_ptr_);
    ConnectionIterator iterator = --connections_.end();

    acceptor_.async_accept(iterator->socket_,
                            boost::bind(&LinkDoveServer::handle_accept,
                                        shared_from_this(),
                                        iterator,
                                        boost::asio::placeholders::error()));
    run_context();
    std::cerr << "here\n";
}

void LinkDoveServer::handle_accept(ConnectionIterator iterator, boost::system::error_code error) {
    std::cerr << "accept\n";
    if (error) {
        // logging
        //connections_.erase(iterator);
    } else {
        // handle new connection
        std::cout << "Connection from: " << iterator->socket_.remote_endpoint().address() << "\n";
    }

    start_accept();
}

void LinkDoveServer::run_context() {
    std::thread t([&]() {
        io_context_ptr_->run();
    });

    t.detach();
}
