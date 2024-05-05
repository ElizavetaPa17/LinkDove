#include "profilewidget.h"
#include "ui_profilewidget.h"

#include "infodialog.h"
#include "constants.h"
#include "clientsingleton.h"

ProfileWidget::ProfileWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProfileWidget)
{
    ui->setupUi(this);
    setupConnection();
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

    QPixmap pix(status_info_.image_path_.c_str());
    pix = pix.scaled(300, 300);
    ui->profileIconLabel->setPixmap(pix);
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

void ProfileWidget::disableEditingMode() {
    ui->editIconLabel->setDisabled(true);
    ui->editIconLabel->hide();
}

void ProfileWidget::setupConnection() {
    connect(ui->editIconLabel, &ClickableLabel::clicked, [this]() { emit editProfile(); });
}
