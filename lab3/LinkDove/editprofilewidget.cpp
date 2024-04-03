#include "editprofilewidget.h"
#include "ui_editprofilewidget.h"

#include <QFileDialog>
#include <QPixmap>

#include <iostream>
#include "constants.h"

EditProfileWidget::EditProfileWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EditProfileWidget)
{
    ui->setupUi(this); 

    setupConnection();
}

EditProfileWidget::~EditProfileWidget()
{
    delete ui;
}

void EditProfileWidget::setStatusInfo(const StatusInfo& status_info) {
    status_info_ = status_info;

    ui->usernameEdit->setText(status_info.username_.c_str());
    ui->emailEdit->setText(status_info.email_.c_str());
    ui->birthdayLabel->setText(status_info.birthday_.toString(BIRTHAY_FORMAT));
    ui->textEdit->setText(status_info.text_status_.c_str());
}

StatusInfo EditProfileWidget::getStatusInfo() {
    return status_info_;
}

void EditProfileWidget::slotChooseUserIcon() {
    QString file_path = QFileDialog::getOpenFileName(nullptr, "Икона пользователя", "", "*.png ;; *.jpg");

    if (!file_path.isEmpty()) {
        QPixmap pixmap(file_path);
        pixmap = pixmap.scaled(300, 300);
        ui->profileIconLabel->setPixmap(pixmap);
    }
}

void EditProfileWidget::setupConnection() {
    connect(ui->okIconLabel,      &ClickableLabel::clicked, this, [this](){ emit editFinished(); });
    connect(ui->profileIconLabel, &ClickableLabel::clicked, this, &EditProfileWidget::slotChooseUserIcon);
}
