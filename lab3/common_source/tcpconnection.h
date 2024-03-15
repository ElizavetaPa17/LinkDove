#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include "IStreamConnection.h"

struct TCPConnection : public IStreamConnection<boost::asio::ip::tcp>
{
public:
    TCPConnection(std::shared_ptr<boost::asio::io_context> io_context_ptr);

    ~TCPConnection() {}
};

#endif // TCPCONNECTION_H
