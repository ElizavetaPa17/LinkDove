#include "userslist.h"
#include "ui_userslist.h"

#include <QStyleOption>
#include <QPainter>
#include <clientsingleton.h>

#include "infodialog.h"

#include <iostream>

UsersList::UsersList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UsersList)
{
    ui->setupUi(this);

    setupConnection();
}

UsersList::~UsersList()
{
    delete ui;
}

void UsersList::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void UsersList::slotsHandleReturnPress() {
    if (ui->searchEdit->text().isEmpty()) {
        // search for all the users
    } else {
        std::cerr << "here";
        ClientSingleton::get_client()->async_find_user(ui->searchEdit->text().toStdString());
    }
}

void UsersList::slotFindUserResult(int result) {
    if (result == FIND_USER_FAILED_ANWSER) {
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>("Пользователь не найден.");
        dialog_ptr->exec();
    } else {
        std::cerr << "found\n";
    }
}

void UsersList::setupConnection() {
    connect(ClientSingleton::get_client(), &Client::find_user_result, this, &UsersList::slotFindUserResult);
    connect(ui->searchEdit, &QLineEdit::returnPressed, this, &UsersList::slotsHandleReturnPress);
}
