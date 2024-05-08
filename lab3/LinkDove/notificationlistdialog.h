#ifndef NOTIFICATIONLISTDIALOG_H
#define NOTIFICATIONLISTDIALOG_H

#include <QDialog>
#include <notification.h>

namespace Ui {
class NotificationListDialog;
}

/**
 * @brief The NotificationListDialog class
 * Модальное окно, отображающее список уведомлений.
 */
class NotificationListDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NotificationListDialog(QWidget *parent = nullptr);
    ~NotificationListDialog();

    /**
     * <p> Добавляет уведомление в отображаемый список. </p>
     * @brief addNotification
     * @param notification - Добавляемое уведомление.
     */
    void addNotification(const Notification &notification);

    /**
     * <p> Удаляет все уведомления из отображаемого списка. </p>
     * @brief removeAllNotifications
     */
    void removeAllNotifications();

public slots:
    /**
     * <p> Слот, обрабатывающий результат удаления уведомления. </p>
     * @brief slotDeleteNotificationResult
     * @param result - Результат удаления уведомления.
     */
    void slotDeleteNotificationResult(int result);

    /**
     * <p> Слот, принимающий сигналы попытки удаления уведомления. </p>
     * @brief slotRemoveNotification
     * @param - Идентификатор удаляемого уведомления.
     */
    void slotRemoveNotification(unsigned long long id);

private:
    Ui::NotificationListDialog *ui;
    unsigned long long removed_notification_id_;

    /**
     * <p> Удаляет уведомление из отображаемого списка. </p>
     * @brief removeNotification
     * @param notification_id - Идентификатор удаляемого уведомления.
     */
    void removeNotification(unsigned long long notification_id);
};

#endif // NOTIFICATIONLISTDIALOG_H
