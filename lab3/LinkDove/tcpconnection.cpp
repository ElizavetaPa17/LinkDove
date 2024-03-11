#include "tcpconnection.h"

TCPConnection::TCPConnection(boost::asio::io_context &io_context)
    : IStreamConnection(io_context)
{}
