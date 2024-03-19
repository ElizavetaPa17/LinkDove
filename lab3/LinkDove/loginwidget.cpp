#include "loginwidget.h"
#include "ui_loginwidget.h"

#include <QMessageBox>

#include "constants.h"

LoginWidget::LoginWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginWidget)
{
    ui->setupUi(this);

    setupConnection();
}

LoginWidget::~LoginWidget()
{
    delete ui;
}

void LoginWidget::slotCheckInput() {
    if (ui->usernameEdit->text().isEmpty() ||
        ui->emailEdit->text().isEmpty()    ||
        ui->passwordEdit->text().isEmpty())
    {
        QMessageBox::information(nullptr, "Некорректный ввод", "Заполните все поля!", QMessageBox::Ok);
    } else {
        emit passLoginWidget(MAIN_PAGE);
    }
}

void LoginWidget::slotSwitchToRegister() {
    emit passLoginWidget(REGISTER_PAGE);
}

void LoginWidget::setupConnection() {
    connect(ui->loginButton,   &QPushButton::clicked,    this, &LoginWidget::slotCheckInput);
    connect(ui->registerLabel, &ClickableLabel::clicked, this, &LoginWidget::slotSwitchToRegister);
}
