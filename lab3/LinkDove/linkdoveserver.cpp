#include "linkdoveserver.h"

#include <thread>
#include <iostream>

LinkDoveServer::LinkDoveServer()
    : acceptor_(io_context_)
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
    connections_.emplace_back(io_context_);
    ConnectionIterator iterator = --connections_.end();

    acceptor_.async_accept(io_context_,
                            boost::bind(&LinkDoveServer::handle_accept,
                                        shared_from_this(),
                                        iterator,
                                        boost::asio::placeholders::error()));
    run_context();
}

void LinkDoveServer::handle_accept(ConnectionIterator iterator, boost::system::error_code error) {
    if (error) {
        // logging
        //connections_.erase(iterator);
    } else {
        // handle new connection
       // std::cout << "Connection from: " << socket.remote_endpoint( ).address( ).to_string( ) << "\n";
    }

    start_accept();
}

void LinkDoveServer::run_context() {
    std::thread t([&]() {
        io_context_.run();
    });

    t.detach();
}
