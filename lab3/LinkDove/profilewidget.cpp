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

void ProfileWidget::slotUpdateUserResult(int update_result) {
    std::string text;
    if (update_result == UPDATE_USER_SUCCESS_ANSWER) {
        text = "Профиль был успешно обновлен.";
    } else {
        text = "Ошибка обновления профиля. Проверьте корректность введенных данных и попытайтесь позже. ";
    }

    std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(text);
    dialog_ptr->exec();
}

void ProfileWidget::setupConnection() {
    connect(ui->editIconLabel, &ClickableLabel::clicked, [this]() { emit editProfile(); });
    connect(ClientSingleton::get_client(),  &Client::update_user_result, this, &ProfileWidget::slotUpdateUserResult);
}
