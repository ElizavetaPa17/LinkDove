#include "loginwidget.h"
#include "ui_loginwidget.h"

#include <QTimer>

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

void LoginWidget::clearLoginInfo() {
    ui->usernameEdit->setText("");
    ui->emailEdit->setText("");
    ui->passwordEdit->setText("");
}

void LoginWidget::displayFailedInfo(const QString& info) {
    ui->registerLabel->setText(info);

    QPalette palette;
    palette.setColor(QPalette::WindowText, Qt::red);
    ui->registerLabel->setPalette(palette);
    QTimer::singleShot(3000, this, &LoginWidget::slotRestoreRegistrationLabel);
}

void LoginWidget::slotCheckInput() {
    if (ui->usernameEdit->text().isEmpty() ||
        ui->emailEdit->text().isEmpty()    ||
        ui->passwordEdit->text().isEmpty())
    {
        displayFailedInfo("Заполните все поля!");
    } else {
        emit passLoginWidget(this, MAIN_PAGE);
    }
}

//void LoginWidget::slotSwitchToRegister() {
//    emit passLoginWidget(this, REGISTER_PAGE);
//}

void LoginWidget::slotRestoreRegistrationLabel() {
    ui->registerLabel->setText("Зарегистрироваться");

    QPalette palette;
    palette.setColor(QPalette::Window, Qt::white);
    ui->registerLabel->setPalette(palette);
}

void LoginWidget::setupConnection() {
    connect(ui->loginButton,   &QPushButton::clicked,    this, &LoginWidget::slotCheckInput);
    connect(ui->registerLabel, &ClickableLabel::clicked, [this] () {emit passLoginWidget(this, REGISTER_PAGE); });
}
