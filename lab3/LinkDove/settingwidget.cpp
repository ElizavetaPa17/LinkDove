#include "settingwidget.h"
#include "ui_settingwidget.h"

#include "infodialog.h"
#include "complaintdialog.h"
#include "constants.h"
#include "clientsingleton.h"

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

        connect(ui->complaintButton, &QPushButton::clicked,    this, &SettingWidget::slotDisplayComplaintDialog);
    } else {
        privileged_mode_ = PRIVILEGED_MODE;

        ui->complaintButton->setText("Жалобы");
        ui->privateButton->hide();
        ui->banButton->show();

        connect(ui->complaintButton, &QPushButton::clicked,    this, &SettingWidget::slotDisplayComplaintList);
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

    std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(text);
    dialog_ptr->exec();
}

void SettingWidget::slotDisplayComplaintDialog() {
    std::unique_ptr<ComplaintDialog> dialog_ptr = std::make_unique<ComplaintDialog>();
    if (dialog_ptr->exec() == QDialog::Accepted) {
        ClientSingleton::get_client()->async_send_complaint(dialog_ptr->getComplaintText());
    }
}

void SettingWidget::slotDisplayComplaintList() {
    // TODO
}

void SettingWidget::slotDisplayAboutDialog() {
    std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(APPLICATION_DESCRIPTION);
    dialog_ptr->exec();
}

void SettingWidget::setupConnections() {
    connect(ui->quitButton,  &QPushButton::clicked, this, &SettingWidget::slotQuitAccount);
    connect(ui->aboutButton, &QPushButton::clicked, this, &SettingWidget::slotDisplayAboutDialog);
    connect(ClientSingleton::get_client(), &Client::complaint_result, this, &SettingWidget::slotComplaintResult);
}
