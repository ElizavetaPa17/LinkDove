#include "client.h"

#include <sstream>
#include <iostream>

Client::Client(asio::io_context &io_context, const boost::asio::ip::address address, uint16_t port)
    : io_context_ptr_(&io_context),
      connection_(io_context),
      endpoint_(address, port),
      is_connected_(false)
{

}

// лучше io_context в отдельный класс вынести и там запустить его работу в отдельном потоке
void Client::async_connect() {
    connection_.socket_.async_connect(endpoint_,
                                      boost::bind(&Client::on_async_connect,
                                                  shared_from_this(),
                                                  asio::placeholders::error));

    run_context();
}

void Client::async_login() {
    if (!is_connected_) {
        return;
    }

   connection_.out_stream_ << create_login_request();
   /*asio::async_write(connection_.socket_, boost::asio::buffer(connection_.buffer_),
                     boost::bind(&Client::on_async_connect,
                                 shared_from_this(),
                                 asio::placeholders::error,
                                 asio::placeholders::bytes_transferred));*/

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
               << "Password: " << password_;

    return str_stream.str();
}

void Client::on_async_connect(boost::system::error_code error) {
    if (error) {
        // handle error
        is_connected_ = false;
    } else {
        // successful connecting
        is_connected_ = true;
    }
}

void Client::on_async_login(boost::system::error_code error, size_t bytes_transferred) {
    if (error) {
        // handle error
    }

    std::cerr << "success" << std::endl;
}

void Client::run_context() {
    std::thread t([&]() {
        io_context_ptr_->run();
    });

    t.detach();
}
