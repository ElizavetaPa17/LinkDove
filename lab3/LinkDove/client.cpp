#include "client.h"

#include <sstream>
#include <iostream>

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

void Client::async_login() {
   connection_.out_stream_ << create_login_request();
   std::cerr << connection_.buffer_.size();
   asio::async_write(connection_.socket_, connection_.buffer_,
                     boost::bind(&Client::handle_async_login,
                                 shared_from_this(),
                                 asio::placeholders::error,
                                 asio::placeholders::bytes_transferred));

   run_context();
}

void Client::setInfo(const std::string &username, const std::string &email, const std::string &password) {
    username_ = username;
    email_ = email;
    password_ = password;
}


std::string Client::create_login_request() {
    std::stringstream str_stream;
    str_stream << "LOGIN\n";
    str_stream << "Username: " << username_ << '\n'
               << "Email: "    << email_    << '\n'
               << "Password: " << password_ << END_OF_REQUEST;

    return str_stream.str();
}

void Client::handle_async_connect(boost::system::error_code error) {
    if (error) {
        std::cerr << "Failed to connect: " << error.value() << ' ' << error.message() << '\n';
        throw std::runtime_error("Cannot connect to the server");
    } else {
        std::cout << "Successfull connection to the server.\n";
        async_login();
    }
}

void Client::handle_async_login(boost::system::error_code error, size_t bytes_transferred) {
    if (error) {
        std::cerr << "Failed to send login request: " << error.value() << ' ' << error.message() << '\n';
        throw std::runtime_error("Failed to send login request");
    }

    if (bytes_transferred > 0) {
        std::cout << "Send login request to the server. Transfer " << bytes_transferred << " bytes.\n";
    }
}

void Client::run_context() {
    std::thread t([&]() {
        io_context_ptr_->run();
    });

    t.detach();
}
