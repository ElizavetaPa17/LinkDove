#include "tcpconnection.h"

TCPConnection::TCPConnection(std::shared_ptr<boost::asio::io_context> io_context_ptr)
    : IStreamConnection(io_context_ptr)
{}
