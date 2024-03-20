#include "mainwindow.h"

#include <QApplication>

/*#include "client.h"
#include "UserInfo.h"

#include <memory>
#include <boost/asio.hpp>*/

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
