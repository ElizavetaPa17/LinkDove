#include "settingwidget.h"
#include "ui_settingwidget.h"

#include "infodialog.h"
#include "complaintdialog.h"
#include "constants.h"
#include "clientsingleton.h"
#include "bandialog.h"

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
        ui->privateButton->show();
        ui->banButton->hide();

        disconnect(ui->complaintButton, &QPushButton::clicked, this, nullptr);
        connect(ui->complaintButton, &QPushButton::clicked,    this, &SettingWidget::slotDisplayComplaintDialog);
    } else {
        privileged_mode_ = PRIVILEGED_MODE;

        ui->complaintButton->setText("Жалобы");
        ui->privateButton->hide();
        ui->banButton->show();

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
    std::unique_ptr<ComplaintDialog> dialog_ptr = std::make_unique<ComplaintDialog>();
    if (dialog_ptr->exec() == QDialog::Accepted) {
        ClientSingleton::get_client()->async_send_complaint(dialog_ptr->getComplaintText());
    }
}

void SettingWidget::slotDisplayComplaintList(int get_complaints_result) {
    if (get_complaints_result == GET_COMPLAINTS_FAILED_ANSWER) {
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Ошибка получения списка жалоб. Попытайтесь позже.");
        dialog_ptr->exec();
        return;
    }

    complaint_dialog_.removeAllComplaints();
    std::vector<Complaint> complaints = ClientSingleton::get_client()->get_complaints();
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
    connect(ClientSingleton::get_client(), &Client::ban_user_result, dialog_ptr.get(), &BanDialog::slotHandleBanResult);
    dialog_ptr->exec();
}

void SettingWidget::setupConnections() {
    connect(ui->quitButton,  &QPushButton::clicked, this, &SettingWidget::slotQuitAccount);
    connect(ui->aboutButton, &QPushButton::clicked, this, &SettingWidget::slotDisplayAboutDialog);
    connect(ui->banButton,   &QPushButton::clicked, this, &SettingWidget::slotDisplayBanDialog);
    connect(ClientSingleton::get_client(), &Client::get_complaints_result, this, &SettingWidget::slotDisplayComplaintList);
    connect(ClientSingleton::get_client(), &Client::send_complaint_result, this, &SettingWidget::slotComplaintResult);
}
