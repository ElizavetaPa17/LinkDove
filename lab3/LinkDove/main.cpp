//#include "mainwindow.h"

//#include <QApplication>

#include "client.h"
#include "UserInfo.h"

#include <memory>
#include <boost/asio.hpp>

int main(int argc, char *argv[])
{
    std::shared_ptr<Client> client_ptr = std::make_shared<Client>(std::make_shared<boost::asio::io_context>(),
                                                                  boost::asio::ip::make_address(LOCAL_ADDRESS),
                                                                  8000);
    client_ptr->async_connect();
    client_ptr->async_register();

    // Попробуй добавить логирование!!!
    /*QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();*/
}
