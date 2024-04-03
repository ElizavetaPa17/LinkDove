#include "registrationwidget.h"
#include "ui_registrationwidget.h"

#include <QDate>
#include <QTimer>

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

UserInfo RegistrationWidget::getRegistrationRequest() {
    UserInfo user_info;

    user_info.status_info_.username_ = ui->usernameEdit->text().toStdString();
    user_info.status_info_.email_    = ui->emailEdit->text().toStdString();
    user_info.status_info_.birthday_ = ui->birthdayDateEdit->date();
    user_info.password_ = ui->passwordEdit->text().toStdString();

    return user_info;
}

void RegistrationWidget::clearRegistrationInfo() {
    ui->usernameEdit->setText("");
    ui->emailEdit->setText("");
    ui->passwordEdit->setText("");
    ui->repeatPasswordEdit->setText("");
}

void RegistrationWidget::displayFailedInfo(const QString& info) {
    ui->loginLabel->setText(info);

    QPalette palette;
    palette.setColor(QPalette::WindowText, Qt::red);
    ui->loginLabel->setPalette(palette);
    QTimer::singleShot(3000, this, &RegistrationWidget::slotRestoreLoginLabel);
}

void RegistrationWidget::slotCheckInput() {
    if (ui->usernameEdit->text().isEmpty() ||
        ui->emailEdit->text().isEmpty()    ||
        ui->passwordEdit->text().isEmpty() ||
        ui->repeatPasswordEdit->text().isEmpty())
    {
        displayFailedInfo("Заполните все поля!");
    } else {
        if (ui->passwordEdit->text() != ui->repeatPasswordEdit->text()) {
            displayFailedInfo("Значения полей пароля должны совпадать!");
        } else {
            emit passRegistrationWidget(this, MAIN_PAGE);
        }
    }
}

//void RegistrationWidget::slotSwitchToLogin() {
//    emit passRegistrationWidget(this, LOGIN_PAGE);
//}

void RegistrationWidget::slotRestoreLoginLabel() {
    ui->loginLabel->setText("Уже есть аккаунт");

    QPalette palette;
    palette.setColor(QPalette::Window, Qt::white);
    ui->loginLabel->setPalette(palette);
}

void RegistrationWidget::setupConnection() {
    connect(ui->registrationButton, &QPushButton::clicked, this, &RegistrationWidget::slotCheckInput);
    connect(ui->loginLabel, &ClickableLabel::clicked, [this] () { emit passRegistrationWidget(this, LOGIN_PAGE); });
}
