#ifndef ISTREAMCONNECTION_H
#define ISTREAMCONNECTION_H

#include <boost/asio.hpp>
#include <iostream>
#include <memory>

template <typename T>
struct IStreamConnection {
public:
    virtual ~IStreamConnection() {}

    boost::asio::basic_stream_socket<T> socket_;
    boost::asio::streambuf buffer_;
    std::ostream out_stream_;
    std::istream in_stream_;

    IStreamConnection(std::shared_ptr<boost::asio::io_context> io_context_ptr)
        : socket_(*io_context_ptr),
          out_stream_(&buffer_),
          in_stream_(&buffer_)
    {}
};

#endif // ISTREAMCONNECTION_H
