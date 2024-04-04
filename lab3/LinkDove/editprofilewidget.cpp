#include "editprofilewidget.h"
#include "ui_editprofilewidget.h"

#include <QFileDialog>
#include <QPixmap>
#include <iostream>
#include <memory>

#include "constants.h"
#include "infodialog.h"

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
    ui->usernameEdit->setText(status_info.username_.c_str());
    ui->emailEdit->setText(status_info.email_.c_str());
    ui->birthdayLabel->setText(status_info.birthday_.toString(BIRTHAY_FORMAT));
    ui->textEdit->setText(status_info.text_status_.c_str());
}

void EditProfileWidget::slotEditFinished() {
    if (ui->usernameEdit->text().isEmpty() ||
        ui->emailEdit->text().isEmpty())
    {
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>("Поле имени и почты не должны быть пустыми.");
        dialog_ptr->exec();
        return;
    }

    StatusInfo status_info;
    status_info.username_    = ui->usernameEdit->text().toStdString();
    status_info.email_       = ui->emailEdit->text().toStdString();
    status_info.birthday_    = QDate::fromString(ui->birthdayLabel->text());
    status_info.text_status_ = ui->textEdit->toPlainText().toStdString();

    emit editFinished(status_info);
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
    connect(ui->okIconLabel,      &ClickableLabel::clicked, this, &EditProfileWidget::slotEditFinished);
    connect(ui->profileIconLabel, &ClickableLabel::clicked, this, &EditProfileWidget::slotChooseUserIcon);
}
