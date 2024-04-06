#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <utility>

#include "clientsingleton.h"
#include "constants.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/**
 * @brief The MainWindow class
 * Виджет, отображающий основную функциональность приложения: профиль, настройки и чаты.
 */
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

    /**
     * <p> Слот, принимающий от клиента результат авторизации и реагирующий соответсвующим образом.</p>
     * @brief slotPassAuthorization
     * @param authorization_result - Результат авторизации.
     */
    void slotPassAuthorization(int authorization_result);

private:
    Ui::MainWindow *ui;

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
