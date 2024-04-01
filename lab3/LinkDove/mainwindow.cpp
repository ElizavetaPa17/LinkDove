    #include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <iostream>
#include <memory>

#include "infodialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->mainStackedWidget->setCurrentIndex(WELCOME_PAGE);

    setupConnection();
    setStyleSheet(" QMainWindow { background-image: url(:/resources/welcome_background.png); }");

    client_ptr->async_connect();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::slotSwitchToPage(QWidget *sender, int index_page) {

    if (sender == ui->pageLogin && index_page == MAIN_PAGE) {
        tryLoginAttempt();
    } else if (sender == ui->pageRegister && index_page == MAIN_PAGE) {
        tryRegisterAttempt();
    } else {
        ui->mainStackedWidget->setCurrentIndex(index_page);
    }
}

void MainWindow::slotPassAuthorization(int authorization_result) {
    switch (authorization_result) {
        case LOGIN_SUCCESS_ANSWER:
        case REGISTRATION_SUCCESS_ANSWER:
            ui->mainStackedWidget->setCurrentIndex(MAIN_PAGE);
            ui->pageMain->setStatusInfo(client_ptr->get_status_info());

            ui->pageLogin->clearLoginInfo();
            ui->pageRegister->clearRegistrationInfo();
            break;
        case LOGIN_FAILED_ANSWER: {
            ui->pageLogin->displayFailedInfo("Ошибка. Проверьте ввод и повторите авторизацию снова.");
            break;
        }
        case REGISTRATION_FAILED_ANSWER: {
            ui->pageRegister->displayFailedInfo("Ошибка. Проверьте ввод и повторите регистрацию снова.");
            break;
        }
    }
}

void MainWindow::slotComplaintResult(int complaint_result) {
    std::string text;
    if (complaint_result == SEND_COMPLAINT_SUCCESS_ANSWER) {
        text = "Ваша жалоба была отправлена.";
    } else {
        text = "Ошибка отправки. Повторите попытку позже.";
    }

    std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(text);
    dialog_ptr->exec();
}


void MainWindow::slotSendComplaint(std::string text) {
    client_ptr->async_send_complaint(text);
}

void MainWindow::setupConnection() {
    connect(ui->pageWelcome,  &WelcomeWidget::passWelcomePage, this, &MainWindow::slotSwitchToPage);
    connect(ui->pageLogin,    &LoginWidget::passLoginWidget,   this, &MainWindow::slotSwitchToPage);
    connect(ui->pageRegister, &RegistrationWidget::passRegistrationWidget, this, &MainWindow::slotSwitchToPage);
    connect(ui->pageMain,     &MainWidget::switchToPage,       this, &MainWindow::slotSwitchToPage);
    connect(ui->pageMain,     &MainWidget::sendComplaint,      this, &MainWindow::slotSendComplaint);

    connect(client_ptr.get(), &Client::authorization_result,   this, &MainWindow::slotPassAuthorization);
    connect(client_ptr.get(), &Client::complaint_result,       this, &MainWindow::slotComplaintResult);
}

void MainWindow::tryLoginAttempt() {
    LoginInfo login_info = ui->pageLogin->getLoginRequest();
    client_ptr->async_login(login_info);
}

void MainWindow::tryRegisterAttempt() {
    UserInfo user_info = ui->pageRegister->getRegistrationRequest();
    client_ptr->async_register(user_info);
}
