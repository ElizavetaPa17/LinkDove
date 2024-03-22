#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <utility>

#include "client.h"
#include "constants.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    /**
     * <p> Слот, принимающий запросы на переключение страницы. </p>
     * @brief slotSwitchToPage
     * @param sender - Указатель на объект, который отправил сигнал.
     * @param index_page - Индекс страницы, на которую необходимо переключиться.
     */
    void slotSwitchToPage(QWidget *sender, int index_page);

private:
    Ui::MainWindow *ui;

    std::shared_ptr<Client> client_ptr = std::make_shared<Client>(std::make_shared<boost::asio::io_context>(),
                                                                      boost::asio::ip::make_address(LOCAL_ADDRESS),
                                                                      8000);

    /**
     * <p> Настраивает соединения с главным виджетом. </p>
     * @brief setupConnection
     */
    void setupConnection();

    /**
     * <p> Осуществляет попытку клиента отправить запрос на авторизацию серверу. <p>
     * @brief tryLoginAttempt
     */
    void tryLoginAttempt();

    /**
     * <p> Осуществляет попытку клиента отправить запрос на регистрацию серверу. <p>
     * @brief tryRegisterAttempt
     */
    void tryRegisterAttempt();

};
#endif // MAINWINDOW_H
