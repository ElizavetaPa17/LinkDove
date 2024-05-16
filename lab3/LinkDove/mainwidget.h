#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>

#include "UserInfo.h"
#include "constants.h"

namespace Ui {
class MainWidget;
}

/**
 * @brief The MainWidget class
 * Виджет отображения главного меню, из которого доступные функциональные вкладки приложения.
 */
class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = nullptr);
    ~MainWidget();

    /**
     * <p> Устанавливает информацию о пользователе. По константе ADMIN_USERNAME устанавливает режим работы приложения (APPLICATION_MODE). </p.
     * @brief setStatusInfo
     * @param status_info - Структура, содержащая информацию о пользователе.
     */
    void setStatusInfo(const StatusInfo& status_info);

signals:
    void switchToPage(QWidget *sender, int index);

public slots:
    /**
     * <p> Слот, принимающий от клиента результат обновления данных о пользователе. </p>
     * @brief slotUpdateUserResult
     * @param update_result - Результат обновления данных о пользователе.
     */
    void slotUpdateUserResult(int update_result);

private slots:
    /**
     * <p> Преобразовывает сигнал клика на функциональные вкладки в сигнал switchToPage на нужную страницу. </p>
     * @brief redirectClick
     * @param sender - Указатель на объект, который отправляет сигнал.
     */
    void slotRedirectClick(QWidget *sender);

    /**
     * <p> Преобразовывает сигналы клика на типы чатов в сигнал switchToPage на нужную страницу.</p>
     * @brief slotTabRedirectClick
     * @param index - Индекс страницы, на которую нажимает пользователь.
     */
    void slotTabRedirectClick(int index);

    /**
     * <p> Принимает сигналы выхода из аккаунта. </p>
     * @brief slotQuit
     */
    void slotQuit();

    /**
     * <p> Слот, который управляет установкой нужных страниц при выходе из групп, каналов или их удалений. </p>
     * @brief slotRedirectTabWidget
     * @param result - Параметр, содержащий результат запроса к серверу.
     */
    void slotRedirectTabWidget(int result);

    /**
     * <p> Слот, который обрабатывает результат получения статуса блокировки пользователя и ограничивает доступ к переписке.</p>
     * @brief slotHandleBannedStatus
     * @param result - Параметр, содержащий результат получения статуса.
     * @param is_banned - Флаг блокировки
     */
    void slotHandleBannedStatus(int result, bool is_banned);

private:
    Ui::MainWidget *ui;
    StatusInfo status_info_;
    int application_mode_ = SIMPLE_MODE;

    /**
     * <p> Устанавливает привилегированный режим работы приложения в зависимости от флага. </p>
     * @brief setPrivilegedMode
     * @param - Если flag = true, то режим привилегированный, иначе - обычный.
     */
    void setPrivilegedMode(bool flag);

    /**
     * <p> Делает виджет навигации доступным. </p>
     * @brief enableNavigationBoard
     */
    void enableNavigationBoard();

    /**
     * <p> Делает виджет навигации недоступным. </p>
     * @brief enableNavigationBoard
     */
    void disableNavigationBoard();

    /**
     * <p> Настраивает соединения с виджетом главного меню. </p>
     * @brief setupConnection
     */
    void setupConnection();
};

#endif // MAINWIDGET_H
