#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <iostream>

#include "constants.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->mainStackedWidget->setCurrentIndex(WELCOME_PAGE);

    setupConnection();
    setStyleSheet(" QMainWindow { background-image: url(:/resources/welcome_background.png); }");
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::slotSwitchToPage(int index_page) {
    ui->mainStackedWidget->setCurrentIndex(index_page);
}

void MainWindow::setupConnection() {
    connect(ui->pageWelcome,  &WelcomeWidget::passWelcomePage, this, &MainWindow::slotSwitchToPage);
    connect(ui->pageLogin,    &LoginWidget::passLoginWidget,   this, &MainWindow::slotSwitchToPage);
    connect(ui->pageRegister, &RegistrationWidget::passRegistrationWidget, this, &MainWindow::slotSwitchToPage);
}
