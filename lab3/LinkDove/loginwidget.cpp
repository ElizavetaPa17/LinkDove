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

std::vector<std::string> LoginWidget::getLoginRequest() {
    std::vector<std::string> login_form_values(3);
    login_form_values[0] = ui->usernameEdit->text().toStdString();
    login_form_values[1] = ui->emailEdit->text().toStdString();
    login_form_values[2] = ui->passwordEdit->text().toStdString();

    return login_form_values;
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
    connect(ui->registerLabel, &ClickableLabel::clicked,    this, &LoginWidget::slotSwitchToRegister);
}
