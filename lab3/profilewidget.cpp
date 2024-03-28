#include "profilewidget.h"
#include "ui_profilepage.h"

ProfileWidget::ProfileWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProfilePage)
{
    ui->setupUi(this);
}

ProfileWidget::~ProfileWidget()
{
    delete ui;
}

void ProfileP::setStatusInfo(const StatusInfo &status_info) {
    ui->usernameLabel->setText(QString::fromUtf8(status_info.username_));
    ui->emailLabel->setText(QString::fromUtf8(status_info.email_));
    ui->birthdayLabel->setText(status_info.birthday_.toString(BIRTHAY_FORMAT));
    ui->textStatusLabel->setText(QString::fromUtf8(status_info.text_status_));
}
