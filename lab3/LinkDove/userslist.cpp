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

void UsersList::paintEvent(QPaintEvent *)
{
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
        // search for all the users
    } else {
        if (ui->searchEdit->text().toStdString() != ClientSingleton::get_client()->get_status_info().username_) {
            ClientSingleton::get_client()->async_find_user(ui->searchEdit->text().toStdString());
        } else {
            std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Имя пользователя не может совпадать с Вашим.");
            dialog_ptr->exec();
        }
    }
}

void UsersList::slotFindUserResult(int result) {
    removeUsers();

    if (result == FIND_USER_FAILED_ANWSER) {
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Пользователь не найден.");
        dialog_ptr->exec();
    } else {
        StatusInfo status_info = ClientSingleton::get_client()->get_found_user();
        addUser(status_info);
    }
}

void UsersList::slotGetInterlocutorsResult(int result) {
    if (result == GET_INTERLOCUTORS_SUCCESS_ANSWER) {
        std::vector<StatusInfo> interlocutors = ClientSingleton::get_client()->get_interlocutors();
        for (int i = 0; i < interlocutors.size(); ++i) {
            addUser(interlocutors[i]);
        }
    } else {
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Ошибка получения сообщений.");
        dialog_ptr->exec();
    }
}

void UsersList::slotHandleUserCardClicked(const StatusInfo &status_info) {
    emit userCardClicked(status_info);
}

void UsersList::setupConnection() {
    connect(ClientSingleton::get_client(), &Client::find_user_result, this, &UsersList::slotFindUserResult);
    connect(ClientSingleton::get_client(), &Client::get_interlocutors_result, this, &UsersList::slotGetInterlocutorsResult);
    connect(ui->searchEdit, &QLineEdit::returnPressed, this, &UsersList::slotsHandleReturnPress);
}
