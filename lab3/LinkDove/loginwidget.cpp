#include "loginwidget.h"
#include "ui_loginwidget.h"

#include <QMessageBox>

#include "clickablelabel.h"
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

LoginInfo LoginWidget::getLoginRequest() {
    LoginInfo login_info;
    login_info.username_ = ui->usernameEdit->text().toStdString();
    login_info.email_    = ui->emailEdit->text().toStdString();
    login_info.password_ = ui->passwordEdit->text().toStdString();

    return login_info;
}

void LoginWidget::slotCheckInput() {
    if (ui->usernameEdit->text().isEmpty() ||
        ui->emailEdit->text().isEmpty()    ||
        ui->passwordEdit->text().isEmpty())
    {
        QMessageBox::information(nullptr, "Некорректный ввод", "Заполните все поля!", QMessageBox::Ok);
    } else {
        emit passLoginWidget(this, MAIN_PAGE);
    }
}

void LoginWidget::slotSwitchToRegister() {
    emit passLoginWidget(this, REGISTER_PAGE);
}

void LoginWidget::setupConnection() {
    connect(ui->loginButton,   &QPushButton::clicked,    this, &LoginWidget::slotCheckInput);
    connect(ui->registerLabel, &ClickableLabel::clicked,    this, &LoginWidget::slotSwitchToRegister);
}
