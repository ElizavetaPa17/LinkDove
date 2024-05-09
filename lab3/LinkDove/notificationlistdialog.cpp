#include "notificationlistdialog.h"
#include "ui_notificationlistdialog.h"

#include "clientsingleton.h"
#include "notificationcard.h"
#include "utility.h"
#include "infodialog.h"

NotificationListDialog::NotificationListDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NotificationListDialog)
{
    ui->setupUi(this);

    connect(ClientSingleton::get_client(), &Client::del_notification_result, this, &NotificationListDialog::slotDeleteNotificationResult);
}

NotificationListDialog::~NotificationListDialog()
{
    delete ui;
}

void NotificationListDialog::addNotification(const Notification &notification) {/*
    NotificationCard *notification_card = new NotificationCard(this, notification);
    ui->scrollArea->widget()->layout()->addWidget(notification_card);

    connect(notification_card, &NotificationCard::removed, this, &NotificationListDialog::slotRemoveNotification);*/
}


void NotificationListDialog::removeAllNotifications() {/*
    QLayout *pvboxLayout = ui->scrollArea->widget()->layout();
    QtUtility::clean_layout(pvboxLayout);*/
}

void NotificationListDialog::slotDeleteNotificationResult(int result) {/*
    std::string text;
    if (result == DEL_NOTIFICATION_SUCCESS_ANSWER) {
        text = "Уведомление было успешно удалено.";
        removeNotification(removed_notification_id_);
    } else {
        text = "Что-то пошло не так при попытке удалить уведомление.";
    }

    std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, text);
    dialog_ptr->exec();*/
}

void NotificationListDialog::slotRemoveNotification(unsigned long long id) {
    removed_notification_id_ = id;
}

void NotificationListDialog::removeNotification(unsigned long long notification_id) {/*
    QLayout *layout = ui->scrollArea->widget()->layout();

    QLayoutItem *widget_item = nullptr;
    for (int i = 0; i < layout->count(); ++i) {
        if (((widget_item = layout->itemAt(i)) != nullptr) && widget_item->widget()) {
            if (static_cast<NotificationCard*>(widget_item->widget())->getNotificationId() == notification_id) {
                layout->removeItem(widget_item);
                delete widget_item->widget();
                delete widget_item;
                break;
            }
        }
    }*/
}
