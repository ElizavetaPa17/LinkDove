#include "profilewidget.h"
#include "ui_profilewidget.h"

#include "constants.h"

ProfileWidget::ProfileWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProfileWidget)
{
    ui->setupUi(this);

    connect(ui->editIconLabel, &ClickableLabel::clicked, [this]() { emit editProfile(); });
}

ProfileWidget::~ProfileWidget()
{
    delete ui;
}

void ProfileWidget::setStatusInfo(const StatusInfo &status_info) {
    status_info_ = status_info;

    ui->usernameLabel->setText(QString::fromUtf8(status_info.username_));
    ui->emailLabel->setText(QString::fromUtf8(status_info.email_));
    ui->birthdayLabel->setText(status_info.birthday_.toString(BIRTHAY_FORMAT));
    ui->textStatusLabel->setText(QString::fromUtf8(status_info.text_status_));
}

StatusInfo ProfileWidget::getStatusInfo() {
    return status_info_;
}

void ProfileWidget::setPrivelegedMode(bool flag) {
    if (flag) {
        ui->editIconLabel->setDisabled(false);
    } else {
        ui->editIconLabel->setDisabled(true);
    }
}
