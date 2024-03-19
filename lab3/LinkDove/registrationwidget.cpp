#include "registrationwidget.h"
#include "ui_registrationwidget.h"

#include <QDate>
#include <QMessageBox>

#include "constants.h"

RegistrationWidget::RegistrationWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RegistrationWidget)
{
    ui->setupUi(this);

    ui->birthdayDateEdit->setDate(QDate::currentDate());
    ui->birthdayDateEdit->setMaximumDate(QDate::currentDate().addYears(-MINIMUM_ALLOWED_AGE));
    ui->birthdayDateEdit->setMinimumDate(QDate::currentDate().addYears(-100));

    setupConnection();
}

RegistrationWidget::~RegistrationWidget()
{
    delete ui;
}

void RegistrationWidget::slotCheckInput() {
    if (ui->usernameEdit->text().isEmpty() ||
        ui->emailEdit->text().isEmpty()    ||
        ui->passwordEdit->text().isEmpty() ||
        ui->repeatPasswordEdit->text().isEmpty())
    {
        QMessageBox::information(nullptr, "Некорректный ввод", "Заполните все поля!", QMessageBox::Ok);
    } else {
        if (ui->passwordEdit->text() != ui->repeatPasswordEdit->text()) {
            QMessageBox::information(nullptr, "Некорректный ввод", "Значения полей пароля должны совпадать!", QMessageBox::Ok);
        } else {
            emit passRegistrationWidget(MAIN_PAGE);
        }
    }
}

void RegistrationWidget::slotSwitchToLogin() {
    emit passRegistrationWidget(LOGIN_PAGE);
}

void RegistrationWidget::setupConnection() {
    connect(ui->registrationButton, &QPushButton::clicked, this, &RegistrationWidget::slotCheckInput);
    connect(ui->loginLabel, &ClickableLabel::clicked, this, &RegistrationWidget::slotSwitchToLogin);
}
