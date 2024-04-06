#ifndef CLIENTSINGLETON_H
#define CLIENTSINGLETON_H

#include <memory>
#include <utility>

#include "client.h"
#include "constants.h"

class ClientSingleton final {
public:
    ClientSingleton(const ClientSingleton&) = delete;
    ClientSingleton& operator=(const ClientSingleton&) = delete;

    static Client* get_client() {
        static ClientSingleton instance_;
        return instance_.client_.get();
    }

private:
    ClientSingleton() {}

    std::shared_ptr<Client> client_ = std::make_shared<Client>(std::make_shared<boost::asio::io_context>(),
                                                       boost::asio::ip::make_address(LOCAL_ADDRESS),
                                                       8000);
};

#endif // CLIENTSINGLETON_H
