#include "profilewidget.h"
#include "ui_profilewidget.h"

#include "constants.h"

ProfileWidget::ProfileWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProfileWidget)
{
    ui->setupUi(this);
}

ProfileWidget::~ProfileWidget()
{
    delete ui;
}

// ДОБАВИТЬ SIMPLE_MODE PRIVILEGED_MODE
void ProfileWidget::setStatusInfo(const StatusInfo &status_info) {
    ui->usernameLabel->setText(QString::fromUtf8(status_info.username_));
    ui->emailLabel->setText(QString::fromUtf8(status_info.email_));
    ui->birthdayLabel->setText(status_info.birthday_.toString(BIRTHAY_FORMAT));
    ui->textStatusLabel->setText(QString::fromUtf8(status_info.text_status_));
}

void ProfileWidget::setPrivelegedMode(bool flag) {
    if (flag) {
        ui->editIconLabel->setDisabled(false);
    } else {
        ui->editIconLabel->setDisabled(true);
    }
}
