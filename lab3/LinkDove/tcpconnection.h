#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include "IStreamConnection.h"

struct TCPConnection : public IStreamConnection<boost::asio::ip::tcp>
{
public:
    TCPConnection(boost::asio::io_context &io_context);

    ~TCPConnection() {}
};

#endif // TCPCONNECTION_H
