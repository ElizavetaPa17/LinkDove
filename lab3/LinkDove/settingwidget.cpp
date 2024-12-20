#include "settingwidget.h"
#include "ui_settingwidget.h"

#include "infodialog.h"
#include "typestringdialog.h"
#include "constants.h"
#include "clientsingleton.h"
#include "bandialog.h"
#include "listlabeldialog.h"
#include <actionsdialog.h>

SettingWidget::SettingWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingWidget),
    privileged_mode_(SIMPLE_MODE)
{
    ui->setupUi(this);

    setupConnections();
}

SettingWidget::~SettingWidget()
{
    delete ui;
}

void SettingWidget::setPrivilegedMode(bool flag) {
    if (flag) {
        privileged_mode_ = SIMPLE_MODE;

        ui->complaintButton->setText("Жалоба");
        ui->notifyButton->show();
        ui->banButton->hide();
        ui->deleteAccountButton->show();
        ui->statisticsButton->hide();

        disconnect(ui->complaintButton, &QPushButton::clicked, this, nullptr);
        connect(ui->complaintButton, &QPushButton::clicked,    this, &SettingWidget::slotDisplayComplaintDialog);
    } else {
        privileged_mode_ = PRIVILEGED_MODE;

        ui->complaintButton->setText("Жалобы");
        ui->notifyButton->hide();
        ui->banButton->show();
        ui->deleteAccountButton->hide();
        ui->statisticsButton->show();

        disconnect(ui->complaintButton, &QPushButton::clicked, this, nullptr);
        connect(ui->complaintButton, &QPushButton::clicked, this, [] () { ClientSingleton::get_client()->async_get_complaints(); });
    }
}

void SettingWidget::slotQuitAccount() {
    emit quitAccount();
}

void SettingWidget::slotComplaintResult(int complaint_result) {
    std::string text;
    if (complaint_result == SEND_COMPLAINT_SUCCESS_ANSWER) {
        text = "Ваша жалоба была отправлена.";
    } else {
        text = "Ошибка отправки. Повторите попытку позже.";
    }

    std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, text);
    dialog_ptr->exec();
}

void SettingWidget::slotDisplayComplaintDialog() {
    std::unique_ptr<TypeStringDialog> dialog_ptr = std::make_unique<TypeStringDialog>(nullptr, "Введите описание жалобы: ");
    if (dialog_ptr->exec() == QDialog::Accepted) {
        ClientSingleton::get_client()->async_send_complaint(dialog_ptr->getString());
    }
}

void SettingWidget::slotDisplayComplaintList(int get_complaints_result, std::vector<Complaint> complaints) {
    if (get_complaints_result == GET_COMPLAINTS_FAILED_ANSWER) {
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Ошибка получения списка жалоб. Попытайтесь позже.");
        dialog_ptr->exec();
        return;
    }

    complaint_dialog_.removeAllComplaints();
    size_t sz = complaints.size();

    if (sz == 0) {
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Список жалоб пуст.");
        dialog_ptr->exec();
    } else {
        for (int i = 0; i < sz; ++i) {
            complaint_dialog_.addComplaint(complaints[i]);
        }
        complaint_dialog_.exec();
    }
}

void SettingWidget::slotDisplayAboutDialog() {
    std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, APPLICATION_DESCRIPTION);
    dialog_ptr->exec();
}

void SettingWidget::slotDisplayBanDialog() {
    std::unique_ptr<BanDialog> dialog_ptr = std::make_unique<BanDialog>();
    if (dialog_ptr->exec() == QDialog::Accepted) {
        std::pair<QString, bool> dialog_pair = dialog_ptr->get_info();

        if (dialog_pair.second) {
            ClientSingleton::get_client()->async_ban_user(dialog_pair.first.toStdString(), true);
        } else {
            ClientSingleton::get_client()->async_ban_user(dialog_pair.first.toStdString(), false);
        }
    }
}

void SettingWidget::slotGetNotificationsResult(int result, std::vector<Notification> notifications) {
    if (result == GET_NOTIFICATIONS_FAILED_ANSWER) {
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Что-то пошло не так при попытке получить уведомления. ");
        dialog_ptr->exec();
        return;
    }

    notification_dialog_.removeAllNotifications();
    size_t sz = notifications.size();

    if (sz == 0) {
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Список уведомлений пуст.");
        dialog_ptr->exec();
    } else {
        for (int i = 0; i < sz; ++i) {
            notification_dialog_.addNotification(notifications[i]);
        }
        notification_dialog_.exec();
    }
}

void SettingWidget::slotHandleBanResult(int result) {
    std::string text;
    if (result == BAN_USER_SUCCESS_ANSWER) {
        text = "Статус блокировки пользователя успешно изменен.";
    } else if (result == BAN_USER_FAILED_ANSWER) {
        text = "Что-то пошло не так при попытке изменить статус блокировки пользователя. ";
    } else {
        return;
    }

    std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, text);
    dialog_ptr->exec();
}

void SettingWidget::slotGetBannedInterlocutorsResult(int result, std::vector<std::string> interlocutors) {
    if (result == GET_BANNED_INTERLOCUTORS_SUCCESS_ANSWER) {
        if (interlocutors.size() == 0) {
            std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Список заблокированных собеседников пуст.");
            dialog_ptr->exec();
        } else {
            std::unique_ptr<ListLabelDialog> dialog_ptr = std::make_unique<ListLabelDialog>(nullptr, interlocutors);
            dialog_ptr->exec();
        }
    } else if (result == GET_BANNED_INTERLOCUTORS_FAILED_ANSWER) {
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Что-то пошло не так при попытке получить список заблокированнх пользователей.");
        dialog_ptr->exec();
    }
}

void SettingWidget::slotDeleteAccount(int result) {
    if (result == DEL_ACCOUNT_SUCCESS_ANSWER) {
        slotQuitAccount();
    } else {
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Что-то пошло не так при попытке удалить аккаунт.");
        dialog_ptr->exec();
    }
}

void SettingWidget::slotGetStatistics() {
    std::unique_ptr<TypeStringDialog> dialog_ptr = std::make_unique<TypeStringDialog>(nullptr, "Введите никнейм пользователя: ");
    if (dialog_ptr->exec() == QDialog::Accepted) {
        ClientSingleton::get_client()->async_get_actions(dialog_ptr->getString());
    }
}

void SettingWidget::slotGetBroadcastResult(int result, std::vector<std::string> notifications) {
    if (result == GET_BROADCAST_SUCCESS_ANSWER) {
        if (notifications.size() == 0) {
            std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Список широковещательных уведомлений пуст.");
            dialog_ptr->exec();
        } else {
            std::unique_ptr<ListLabelDialog> dialog_ptr = std::make_unique<ListLabelDialog>(nullptr, notifications);
            dialog_ptr->exec();
        }
    } else {
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Что-то пошло не так при попытке получить список широковещательных сообщений.");
        dialog_ptr->exec();
    }
}

void SettingWidget::slotGetStatisticsResult(int result, std::vector<Action> actions) {
    if (result == GET_ACTIONS_SUCCESS_ANSWER) {
        if (actions.size() == 0) {
            std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Список действий пользователя пуст.");
            dialog_ptr->exec();
        } else {
            std::unique_ptr<ActionsDialog> dialog_ptr = std::make_unique<ActionsDialog>(nullptr, actions);
            dialog_ptr->exec();
        }
    } else {
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Что-то пошло не так при попытке получить список действий пользователя.");
        dialog_ptr->exec();
    }
}

void SettingWidget::setupConnections() {
    connect(ui->quitButton,       &QPushButton::clicked, this, &SettingWidget::slotQuitAccount);
    connect(ui->quitButton,       &QPushButton::clicked, this, [this] () { ClientSingleton::get_client()->async_quit_account(); });
    connect(ui->aboutButton,      &QPushButton::clicked, this, &SettingWidget::slotDisplayAboutDialog);
    connect(ui->banButton,        &QPushButton::clicked, this, &SettingWidget::slotDisplayBanDialog);
    connect(ui->notifyButton,     &QPushButton::clicked, this, [this] () { ClientSingleton::get_client()->async_get_notifications(); });
    connect(ui->confidenceButton, &QPushButton::clicked, this, [this] () { ClientSingleton::get_client()->async_get_banned_interlocutors(); });
    connect(ui->deleteAccountButton, &QPushButton::clicked, this, [this] () { ClientSingleton::get_client()->async_delete_account(); });
    connect(ui->broadcastNotificationButton, &QPushButton::clicked, this, [this] () { ClientSingleton::get_client()->async_get_broadcast_notifications(); });
    connect(ui->statisticsButton, &QPushButton::clicked, this, &SettingWidget::slotGetStatistics);

    connect(ClientSingleton::get_client(), &Client::get_complaints_result,    this, &SettingWidget::slotDisplayComplaintList);
    connect(ClientSingleton::get_client(), &Client::send_complaint_result,    this, &SettingWidget::slotComplaintResult);
    connect(ClientSingleton::get_client(), &Client::get_notifications_result, this, &SettingWidget::slotGetNotificationsResult);
    connect(ClientSingleton::get_client(), &Client::ban_user_result,          this, &SettingWidget::slotHandleBanResult);
    connect(ClientSingleton::get_client(), &Client::get_banned_users,         this, &SettingWidget::slotGetBannedInterlocutorsResult);
    connect(ClientSingleton::get_client(), &Client::delete_account_result,    this, &SettingWidget::slotDeleteAccount);
    connect(ClientSingleton::get_client(), &Client::get_broadcast_notifications_result, this, &SettingWidget::slotGetBroadcastResult);
    connect(ClientSingleton::get_client(), &Client::get_user_actions, this, &SettingWidget::slotGetStatisticsResult);
}
