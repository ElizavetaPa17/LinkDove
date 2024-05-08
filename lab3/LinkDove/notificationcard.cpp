#include "notificationcard.h"
#include "ui_notificationcard.h"

#include "clientsingleton.h"

NotificationCard::NotificationCard(QWidget *parent, const Notification &notification)
    : QWidget(parent)
    , ui(new Ui::NotificationCard)
{
    ui->setupUi(this);

    notification_id_ = notification.id_;
    ui->textLabel->setText(notification.text_.c_str());

    setupConnections();
}

NotificationCard::~NotificationCard()
{
    delete ui;
}

unsigned long long NotificationCard::getNotificationId() {
    return notification_id_;
}

void NotificationCard::setupConnections() {
    connect(ui->delButton, &QPushButton::clicked, this, [this]() { ClientSingleton::get_client()->async_del_notification(notification_id_);
                                                                    std::cerr << notification_id_ << '\n';
                                                                   emit removed(notification_id_);
                                                                  });
}
