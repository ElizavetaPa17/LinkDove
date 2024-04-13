#include "mainwidget.h"
#include "ui_mainwidget.h"

#include <memory>
#include <iostream>

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);

    ui->stackedWidget->setCurrentIndex(EMPTY_PAGE);
    ui->profileStackedWidget->setCurrentIndex(SIMPLE_PROFILE_PAGE);
    setupConnection();
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::setStatusInfo(const StatusInfo& status_info) {
    status_info_ = status_info;
    ui->profileWidget->setStatusInfo(status_info);

    if (status_info.id_ == ADMIN_ID) {
        setPrivilegedMode(PRIVILEGED_MODE);
    } else {
        setPrivilegedMode(SIMPLE_MODE);
    }
}

void MainWidget::slotRedirectClick(QWidget *sender) {
    if (sender == ui->profileLabel) {
        ui->stackedWidget->setCurrentIndex(PROFILE_PAGE);
    } else if (sender == ui->chatLabel) {
        ui->stackedWidget->setCurrentIndex(CHAT_PAGE);
    } else if (sender == ui->settingLabel) {
        ui->stackedWidget->setCurrentIndex(SETTING_PAGE);
    }
}

void MainWidget::slotQuit() {
    // clear status info value
    //memset(&status_info_, 0, sizeof(status_info_));

    ui->stackedWidget->setCurrentIndex(EMPTY_PAGE);
    emit switchToPage(this, LOGIN_PAGE);
}

void MainWidget::slotOpenChatWith(const StatusInfo &status_info) {
    std::cerr << "here\n";
}

void MainWidget::setPrivilegedMode(bool flag) {
    ui->profileWidget->setPrivelegedMode(flag);
    ui->settingWidget->setPrivilegedMode(flag);
}

void MainWidget::setupConnection() {
    connect(ui->profileLabel,    &ClickableLabel::clicked,     this, &MainWidget::slotRedirectClick);
    connect(ui->chatLabel,       &ClickableLabel::clicked,     this, &MainWidget::slotRedirectClick);
    connect(ui->settingLabel,    &ClickableLabel::clicked,     this, &MainWidget::slotRedirectClick);
    connect(ui->settingWidget,   &SettingWidget::quitAccount,  this, &MainWidget::slotQuit);

    connect(ui->profileWidget,     &ProfileWidget::editProfile, [this] () {
                                                                    ui->profileStackedWidget->setCurrentIndex(EDITED_PROFILE_PAGE);
                                                                    ui->profileEditWidget->setStatusInfo(ui->profileWidget->getStatusInfo());
                                                                } );
    connect(ui->profileEditWidget, &EditProfileWidget::editFinished, [this] () {
                                                                         ui->profileStackedWidget->setCurrentIndex(SIMPLE_PROFILE_PAGE);
                                                                         ui->profileWidget->setStatusInfo(ui->profileEditWidget->getStatusInfo());
                                                                     } );

    connect(ui->usersList, &UsersList::userCardClicked, this, &MainWidget::slotOpenChatWith);
}
