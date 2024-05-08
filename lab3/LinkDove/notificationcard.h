#ifndef NOTIFICATIONCARD_H
#define NOTIFICATIONCARD_H

#include <QWidget>

#include "notification.h"

namespace Ui {
class NotificationCard;
}

/**
 * @brief The NotificationCard class
 * Виджет, отображающий информацию об уведомлении.
 */
class NotificationCard : public QWidget
{
    Q_OBJECT

public:
    explicit NotificationCard(QWidget *parent, const Notification &notification);
    ~NotificationCard();

    /**
     * <p> Возвращает идентификатор отображаемого уведомления. </p>
     * @brief getNotificationId
     * @return
     */
    unsigned long long getNotificationId();

signals:
    /**
     * <p> Сигнал генерируется при попытке удалить уведомление. /p>
     * @brief removed
     * @param id - Идентификатор удаляемого уведомления.
     */
    void removed(unsigned long long id);

private:
    Ui::NotificationCard *ui;
    unsigned long long notification_id_;

    /**
     * <p> Настраивает соединения внутри виджета. </P>
     * @brief setupConnection
     */
    void setupConnections();
};

#endif // NOTIFICATIONCARD_H
