#include "mainwidget.h"
#include "ui_mainwidget.h"

#include <memory>
#include <iostream>

#include "clientsingleton.h"
#include "infodialog.h"

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

        if (ui->tabWidget->currentIndex() == 0) { // FIX MAGIC INT
            ClientSingleton::get_client()->async_get_interlocutors();
        } else if (ui->tabWidget->currentIndex() == 2) { // FIX MAGIN INT
            ClientSingleton::get_client()->async_get_channels();
        } else {
            ClientSingleton::get_client()->async_get_chats();
        }

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

void MainWidget::setPrivilegedMode(bool flag) {
    ui->profileWidget->setPrivelegedMode(flag);
    ui->settingWidget->setPrivilegedMode(flag);
}

void MainWidget::enableNavigationBoard() {
    ui->profileLabel->setEnabled(true);
    ui->chatLabel->setEnabled(true);
    ui->settingLabel->setEnabled(true);
}

void MainWidget::disableNavigationBoard() {
    ui->profileLabel->setEnabled(false);
    ui->chatLabel->setEnabled(false);
    ui->settingLabel->setEnabled(false);
}


void MainWidget::slotUpdateUserResult(int update_result) {
    std::string text;
    if (update_result == UPDATE_USER_SUCCESS_ANSWER) {
        text = "Профиль был успешно обновлен.";
        ui->profileWidget->setStatusInfo(ui->profileEditWidget->getStatusInfo());
    } else {
        text = "Ошибка обновления профиля. Проверьте корректность введенных данных и попытайтесь позже. ";
    }

    std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, text);
    dialog_ptr->exec();

    ui->profileStackedWidget->setCurrentIndex(SIMPLE_PROFILE_PAGE);
    enableNavigationBoard();
}

void MainWidget::setupConnection() {
    connect(ui->profileLabel,    &ClickableLabel::clicked,     this, &MainWidget::slotRedirectClick);
    connect(ui->chatLabel,       &ClickableLabel::clicked,     this, &MainWidget::slotRedirectClick);
    connect(ui->settingLabel,    &ClickableLabel::clicked,     this, &MainWidget::slotRedirectClick);

    // При выходе из аккаунта сбрасываем все содержимое виджетов
    connect(ui->settingWidget,   &SettingWidget::quitAccount,  this, &MainWidget::slotQuit);
    connect(ui->settingWidget,   &SettingWidget::quitAccount,  ui->chatWidget, &ChatWidget::slotClear);
    connect(ui->settingWidget,   &SettingWidget::quitAccount,  ui->usersList,   &UsersList::slotClear);
    connect(ui->settingWidget,   &SettingWidget::quitAccount,  ui->channelList, &ChannelList::slotClear);
    connect(ui->settingWidget,   &SettingWidget::quitAccount,  ui->channelWidget, &ChannelWidget::slotClear);
    connect(ui->settingWidget,   &SettingWidget::quitAccount,  ui->groupWidget, &GroupWidget::slotClear);

    connect(ui->profileWidget,     &ProfileWidget::editProfile, [this] () {
                                                                    ui->profileStackedWidget->setCurrentIndex(EDITED_PROFILE_PAGE);
                                                                    ui->profileEditWidget->setStatusInfo(ui->profileWidget->getStatusInfo());
                                                                    disableNavigationBoard();
                                                                } );
    connect(ClientSingleton::get_client(), &Client::update_user_result, this, &MainWidget::slotUpdateUserResult);

    connect(ui->usersList, &UsersList::userCardClicked, ui->chatWidget, &ChatWidget::slotOpenChatWith);
    connect(ui->channelList, &ChannelList::channelCardClicked, ui->channelWidget, &ChannelWidget::slotOpenChannel);
    connect(ui->chatList, &ChatList::chatCardClicked, ui->groupWidget, &GroupWidget::slotOpenGroup);
}
