#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <iostream>

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

void MainWindow::setupConnection() {
    connect(ui->pageWelcome,  &WelcomeWidget::passWelcomePage, this, &MainWindow::slotSwitchToPage);
    connect(ui->pageLogin,    &LoginWidget::passLoginWidget,   this, &MainWindow::slotSwitchToPage);
    connect(ui->pageRegister, &RegistrationWidget::passRegistrationWidget, this, &MainWindow::slotSwitchToPage);
}

void MainWindow::tryLoginAttempt() {
    LoginInfo login_info = ui->pageLogin->getLoginRequest();
    client_ptr->async_login(login_info);
}

void MainWindow::tryRegisterAttempt() {
    UserInfo user_info = ui->pageRegister->getRegistrationRequest();
    client_ptr->async_register(user_info);
}
