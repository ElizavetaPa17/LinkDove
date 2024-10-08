#include "userslist.h"
#include "ui_userslist.h"

#include <QStyleOption>
#include <QPainter>
#include <QVBoxLayout>

#include "clientsingleton.h"
#include "infodialog.h"
#include "usercard.h"
#include "utility.h"

#include <iostream>

UsersList::UsersList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UsersList)
{
    ui->setupUi(this);
    ui->scrollArea->setWidget(new QWidget());
    ui->scrollArea->widget()->setLayout(new QVBoxLayout());
    ui->scrollArea->widget()->layout()->setAlignment(Qt::AlignTop);

    setupConnection();
}

UsersList::~UsersList()
{
    delete ui;
}

void UsersList::slotClear() {
    removeUsers();
}

void UsersList::paintEvent(QPaintEvent *) {
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void UsersList::addUser(const StatusInfo &status_info) {
    UserCard *user_card = new UserCard(this, status_info);
    connect(user_card, &UserCard::userCardClicked, this, &UsersList::slotHandleUserCardClicked);

    ui->scrollArea->widget()->layout()->addWidget(user_card);
}

void UsersList::removeUsers() {
    ui->searchEdit->setText("");
    QtUtility::clean_layout(ui->scrollArea->widget()->layout());
}

void UsersList::slotsHandleReturnPress() {
    if (ui->searchEdit->text().isEmpty()) {
        ClientSingleton::get_client()->async_get_interlocutors();
    } else {
        if (ui->searchEdit->text().toStdString() != ADMIN_USERNAME) {
            ClientSingleton::get_client()->async_find_user(ui->searchEdit->text().toStdString());
            return;
        } else {
            std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Вы не можете написать администратору, пока он не напишет вам первым.");
            dialog_ptr->exec();
            return;
        }

        if (ui->searchEdit->text().toStdString() != ClientSingleton::get_client()->get_status_info().username_) {
            ClientSingleton::get_client()->async_find_user(ui->searchEdit->text().toStdString());
        } else {
            std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Имя пользователя не может совпадать с Вашим.");
            dialog_ptr->exec();
        }
    }
}

void UsersList::slotFindUserResult(int result) {
    if (result == FIND_USER_FAILED_ANWSER) {
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Пользователь не найден.");
        dialog_ptr->exec();
    } else {

        StatusInfo status_info = ClientSingleton::get_client()->get_found_user();
        removeUsers();
        addUser(status_info);
    }
}

void UsersList::slotGetInterlocutorsResult(int result, std::vector<StatusInfo> interlocutors) {
     removeUsers();

    if (result == GET_INTERLOCUTORS_SUCCESS_ANSWER) {
        for (int i = 0; i < interlocutors.size(); ++i) {
            addUser(interlocutors[i]);
        }
    } else {
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Ошибка получения сообщений.");
        dialog_ptr->exec();
    }
}

void UsersList::slotHandleUserCardClicked(const StatusInfo &status_info) {
    status_info_ = status_info;

    ClientSingleton::get_client()->async_is_banned_ind_user(status_info.username_);
}

void UsersList::slotHandleIsBannedUser(int result, bool is_banned) {
    if (result == IS_IND_BANNED_USER_SUCCESS_ANSWER) {
        if (!is_banned) {
            emit userCardClicked(status_info_);
        } else {
            std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Вы были заблокированы пользователем. ");
            dialog_ptr->exec();
        }
    } else if (result == IS_IND_BANNED_USER_FAILED_ANSWER) {
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Что-то пошло не так при попытке получить статус блокировки в переписке. ");
        dialog_ptr->exec();
    }
}

void UsersList::setupConnection() {
    connect(ClientSingleton::get_client(), &Client::find_user_result,         this, &UsersList::slotFindUserResult);
    connect(ClientSingleton::get_client(), &Client::get_interlocutors_result, this, &UsersList::slotGetInterlocutorsResult);
    connect(ClientSingleton::get_client(), &Client::is_banned_user_result,    this, &UsersList::slotHandleIsBannedUser);

    connect(ui->searchEdit,                &QLineEdit::returnPressed,         this, &UsersList::slotsHandleReturnPress);
}
