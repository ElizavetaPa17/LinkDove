//#include "mainwindow.h"

#include <QApplication>

#include "linkdoveserver.h"
#include <memory>
#include <thread>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    /*std::shared_ptr<LinkDoveServer> server_ptr = std::make_shared<LinkDoveServer>();

    server_ptr->listen(8000);
    while(true) {
        std::this_thread::yield();
    }*/

    boost::asio::streambuf b;
    std::ostream os(&b);
    std::istream is(&b);

}
