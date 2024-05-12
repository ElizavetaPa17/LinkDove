#include "editprofilewidget.h"
#include "ui_editprofilewidget.h"

#include <QFileDialog>
#include <QPixmap>
#include <iostream>
#include <memory>

#include "constants.h"
#include "infodialog.h"
#include "clientsingleton.h"
#include "utility.h"

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

    QPixmap pix(status_info_.image_path_.c_str());
    pix = pix.scaled(300, 300);
    ui->profileIconLabel->setPixmap(pix);
}

StatusInfo EditProfileWidget::getStatusInfo() {
    return status_info_;
}

void EditProfileWidget::slotEditFinished() {
    if (ui->usernameEdit->text().isEmpty() ||
        ui->emailEdit->text().isEmpty())
    {
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Поле имени и почты не должны быть пустыми.");
        dialog_ptr->exec();
        return;
    }

    status_info_.username_    = ui->usernameEdit->text().toStdString();
    status_info_.email_       = ui->emailEdit->text().toStdString();
    status_info_.birthday_    = QDate::fromString(ui->birthdayLabel->text());
    status_info_.text_status_ = ui->textEdit->toPlainText().toStdString();

    ClientSingleton::get_client()->async_update_user(status_info_);
    emit editFinished();
}

void EditProfileWidget::slotChooseUserIcon() {
    QString file_path = QFileDialog::getOpenFileName(nullptr, "Икона пользователя", "", "*.png ;; *.jpg");

    file_path = MessageUtility::copy_image_to_avatars_folder(file_path).c_str();

    if (!file_path.isEmpty()) {
        QPixmap pixmap(file_path);
        pixmap = pixmap.scaled(300, 300);
        ui->profileIconLabel->setPixmap(pixmap);

        status_info_.image_path_ = file_path.toStdString();
    }
}

void EditProfileWidget::setupConnection() {
    connect(ui->okIconLabel,      &ClickableLabel::clicked, this, &EditProfileWidget::slotEditFinished);
    connect(ui->profileIconLabel, &ClickableLabel::clicked, this, &EditProfileWidget::slotChooseUserIcon);
}
