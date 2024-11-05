#ifndef SETTINGWIDGET_H
#define SETTINGWIDGET_H

#include <QWidget>

#include "complaintslistdialog.h"
#include "notificationlistdialog.h"
#include "notification.h"

namespace Ui {
class SettingWidget;
}

/**
 * @brief The SettingWidget class
 * Класс, отвечающий за настройки и дополнительный функционал приложения.
 */
class SettingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SettingWidget(QWidget *parent = nullptr);
    ~SettingWidget();    

signals:
    /**
     * <p> Генерирует сигнал выхода из аккаунта. </p>
     * @brief quitAccount
     */
    void quitAccount();

    /**
     * <p> Генерирует сигнал для отправки жалобы. </p>
     * @brief sendComplaint
     * @param complaint_text - Содержание жалобы.
     */
    //void sendComplaint(std::string complaint_text);

public slots:
    /**
     * <p> Устанавливает привилегированный режим работы в зависимости от флага. </p>
     * @brief setPrivelegedMode
     * @param flag - Если flag = true, то режим привилегированный, иначе - обычный.
     */
    void setPrivilegedMode(bool flag);

    /**
     * <p> Слот, принимающий от клиента результат отправки жалобы. </p>
     * @brief slotComplaintResult
     * @param complaint_result - Результат отправки жалобы.
     */
    void slotComplaintResult(int complaint_result);

private slots:
    /**
     * <p> Принимает сигнал кнопки нажатия выхода и генерирует сигнал выхода. </p>
     * @brief slotQuitAccount
     */
    void slotQuitAccount();

    /**
     * <p> Слот, отображающий модальное диалоговое окно для написания жалобы админу. </p>
     * @brief slotDisplayComplaintDialog
     */
    void slotDisplayComplaintDialog();

    /**
     * <p> Слот, отображающий список жалоб админу. </p>
     * @brief slotDisplayComplaintList
     * @param get_complaints_result - Результат получения списка жалоб.
     * @param complaints - Список жалоб.
     */
    void slotDisplayComplaintList(int complaints_result, std::vector<Complaint> complaints);

    /**
     * <p> Отображает модальное окно с информацией о приложении. </p>
     * @brief slotDisplayAboutDialog
     */
    void slotDisplayAboutDialog();

    /**
     * <p> Отображает модальное окно блокировки для суперпользователя. </p>
     * @brief slotDisplayBanDialog
     */
    void slotDisplayBanDialog();

    /**
     * <p> Слот, обрабатывающий результат запроса получения списка уведомлений. </p>
     * @brief slotGetNotificationsResult
     * @param result - Парметр, содержащий результат запроса.
     * @param notifications - Список уведомлений.
     */
    void slotGetNotificationsResult(int result, std::vector<Notification> notifications);

    /**
     * <p> Слот, обрабатывающий результат блокировки/разблокировки пользователя. </p>
     * @brief slotHandleBanResult
     * @param result
     */
    void slotHandleBanResult(int result);

    /**
     * <p> Слот, обрабатывающий результат получения списка никнеймов заблокированных пользователей. </p>
     * @brief slotGetBannedInterlocutorsResult
     * @param result - Парметр, содержащий результат запроса.
     * @param interlocutors - Список никнеймов заблокированных пользователей.
     */
    void slotGetBannedInterlocutorsResult(int result, std::vector<std::string> interlocutors);

    /**
     * <p> Слот, обрабатывающий результат удаления аккаунта. </p>
     * @brief slotDeleteAccount
     * @param result
     */
    void slotDeleteAccount(int result);

    void slotGetBroadcastResult(int result, std::vector<std::string> notifications);

private:
    Ui::SettingWidget *ui;
    ComplaintsListDialog complaint_dialog_;
    NotificationListDialog notification_dialog_;

    int privileged_mode_;

    /**
     * <p> Настраивает соединения с классом. </p>
     * @brief setupConnections
     */
    void setupConnections();
};

#endif // SETTINGWIDGET_H
