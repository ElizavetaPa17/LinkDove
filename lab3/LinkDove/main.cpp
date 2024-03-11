//#include "mainwindow.h"

//#include <QApplication>
#include "linkdoveserver.h"
#include <memory>

int main(int argc, char *argv[])
{
    auto server_ptr = std::make_shared<LinkDoveServer>();

    // Попробуй добавить логирование!!!
    /*QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();*/
}
