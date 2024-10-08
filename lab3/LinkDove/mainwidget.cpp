#include "mainwidget.h"
#include "ui_mainwidget.h"

#include <memory>
#include <iostream>

#include "clientsingleton.h"
#include "infodialog.h"
#include "audiomanagersingleton.h"

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
    AudioManagerSingleton::get_manager()->stop();

    if (sender == ui->profileLabel) {
        ui->stackedWidget->setCurrentIndex(PROFILE_PAGE);
    } else if (sender == ui->chatLabel) {
        if (ClientSingleton::get_client()->get_status_info().is_banned_) {
            std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Ваш аккаунт был заблокирован. Для выяснения причины свяжитесь с администратором. ");
            dialog_ptr->exec();

            return;
        }

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

void MainWidget::slotTabRedirectClick(int index) {
    if (index == 0 /*INDIVIDUA _MESSAGES PAGE*/) {
        ClientSingleton::get_client()->async_get_interlocutors();
    } else if (index == 1 /*CHATS (GROUPS) PAGE*/) {
        ClientSingleton::get_client()->async_get_chats();
    } else /* CHANNELS PAGE */{
        ClientSingleton::get_client()->async_get_channels();
    }
}

void MainWidget::slotQuit() {
    ui->chatStackedWidget->setCurrentIndex(1);
    ui->groupStackedWidget->setCurrentIndex(1);
    ui->channelStackedWidget->setCurrentIndex(1);

    ui->stackedWidget->setCurrentIndex(EMPTY_PAGE);
    emit switchToPage(this, LOGIN_PAGE);
}

void MainWidget::slotRedirectTabWidget(int result) {
    if (result == DELETE_CHANNEL_SUCCESS_ANSWER || result == QUIT_CHANNEL_SUCCESS_ANSWER) {
        ui->channelStackedWidget->setCurrentIndex(1);
        ClientSingleton::get_client()->async_get_channels();
    } else if (result == DELETE_CHAT_SUCCESS_ANSWER || result == QUIT_CHAT_SUCCESS_ANSWER) {
        ui->groupStackedWidget->setCurrentIndex(1);
        ClientSingleton::get_client()->async_get_chats();
    } else if (result == DELETE_IND_CHAT_SUCCESS_ANSWER) {
        ui->chatStackedWidget->setCurrentIndex(1);
        ClientSingleton::get_client()->async_get_interlocutors();
    }
}

void MainWidget::slotHandleBannedStatus(int result, bool is_banned) {
    switch (result) {
        case IS_IND_BANNED_USER_FAILED_ANSWER: {
            ui->chatStackedWidget->setCurrentIndex(1);
            break;
        }
        case IS_CHNNL_BANNED_USER_FAILED_ANSWER: {
            ui->channelStackedWidget->setCurrentIndex(1);
            break;
        }
        case IS_CHAT_BANNED_USER_FAILED_ANSWER: {
            ui->groupStackedWidget->setCurrentIndex(1);
            break;
        }
        case IS_IND_BANNED_USER_SUCCESS_ANSWER: {
            if (is_banned) {
                ui->chatStackedWidget->setCurrentIndex(1);
            }
            break;
        }
        case IS_CHNNL_BANNED_USER_SUCCESS_ANSWER: {
            if (is_banned) {
                ui->channelStackedWidget->setCurrentIndex(1);
            }
            break;
        }
        case IS_CHAT_BANNED_USER_SUCCESS_ANSWER: {
            if (is_banned) {
                ui->groupStackedWidget->setCurrentIndex(1);
            }
            break;
        }
    }
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

    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &MainWidget::slotTabRedirectClick);

    connect(ui->channelWidget, &ChannelWidget::openChannelWidget, this, [this]() {ui->channelStackedWidget->setCurrentIndex(0);});
    connect(ui->chatWidget,    &ChatWidget::openChatWidget,       this, [this]() {ui->chatStackedWidget->setCurrentIndex(0);});
    connect(ui->groupWidget,   &GroupWidget::openGroupWidget,     this, [this]() {ui->groupStackedWidget->setCurrentIndex(0);});

    // При выходе из аккаунта сбрасываем все содержимое виджетов
    connect(ui->settingWidget,   &SettingWidget::quitAccount,  this, &MainWidget::slotQuit);
    connect(ui->settingWidget,   &SettingWidget::quitAccount,  ui->usersList,     &UsersList::slotClear);
    connect(ui->settingWidget,   &SettingWidget::quitAccount,  ui->channelList,   &ChannelList::slotClear);
    connect(ui->settingWidget,   &SettingWidget::quitAccount,  ui->chatList,      &ChatList::slotClear);
    connect(ui->settingWidget,   &SettingWidget::quitAccount,  ui->chatWidget,    &ChatWidget::slotClear);
    connect(ui->settingWidget,   &SettingWidget::quitAccount,  ui->channelWidget, &ChannelWidget::slotClear);
    connect(ui->settingWidget,   &SettingWidget::quitAccount,  ui->groupWidget,   &GroupWidget::slotClear);

    connect(ui->profileWidget,     &ProfileWidget::editProfile, [this] () {
                                                                    ui->profileStackedWidget->setCurrentIndex(EDITED_PROFILE_PAGE);
                                                                    ui->profileEditWidget->setStatusInfo(ui->profileWidget->getStatusInfo());
                                                                    disableNavigationBoard();
                                                                } );
    connect(ClientSingleton::get_client(), &Client::update_user_result,    this, &MainWidget::slotUpdateUserResult);


    connect(ClientSingleton::get_client(), &Client::delete_channel_result,  this, &MainWidget::slotRedirectTabWidget);
    connect(ClientSingleton::get_client(), &Client::delete_chat_result,     this, &MainWidget::slotRedirectTabWidget);
    connect(ClientSingleton::get_client(), &Client::delete_ind_chat_result, this, &MainWidget::slotRedirectTabWidget);
    connect(ClientSingleton::get_client(), &Client::quit_chat_result,       this, &MainWidget::slotRedirectTabWidget);
    connect(ClientSingleton::get_client(), &Client::quit_channel_result,    this, &MainWidget::slotRedirectTabWidget);
    connect(ClientSingleton::get_client(), &Client::is_banned_user_result,  this, &MainWidget::slotHandleBannedStatus);


    connect(ui->usersList,   &UsersList::userCardClicked,      ui->chatWidget,    &ChatWidget::slotOpenChatWith);
    connect(ui->channelList, &ChannelList::channelCardClicked, ui->channelWidget, &ChannelWidget::slotOpenChannel);
    connect(ui->chatList,    &ChatList::chatCardClicked,       ui->groupWidget,   &GroupWidget::slotOpenGroup);
}
