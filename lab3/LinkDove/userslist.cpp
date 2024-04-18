#include "userslist.h"
#include "ui_userslist.h"

#include <QStyleOption>
#include <QPainter>
#include <QVBoxLayout>

#include "clientsingleton.h"
#include "infodialog.h"
#include "usercard.h"

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
    QLayout *pvboxLayout = ui->scrollArea->widget()->layout();
    size_t count = pvboxLayout->count();
    QLayoutItem *widget_item = nullptr;

    while (widget_item = (pvboxLayout->takeAt(0))) {
        if (widget_item != nullptr) {
            delete widget_item;
        }
    }
}

void UsersList::slotsHandleReturnPress() {
    if (ui->searchEdit->text().isEmpty()) {
        // search for all the users
    } else {
        if (ui->searchEdit->text().toStdString() != ClientSingleton::get_client()->get_status_info().username_) {
            ClientSingleton::get_client()->async_find_user(ui->searchEdit->text().toStdString());
        } else {
            std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>("Имя пользователя не может совпадать с Вашим.");
            dialog_ptr->exec();
        }
    }
}

void UsersList::slotFindUserResult(int result) {
    if (result == FIND_USER_FAILED_ANWSER) {
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>("Пользователь не найден.");
        dialog_ptr->exec();
    } else {
        StatusInfo status_info = ClientSingleton::get_client()->get_found_user();
        removeUsers();
        addUser(status_info);
    }
}

void UsersList::slotHandleUserCardClicked(const StatusInfo &status_info) {
    emit userCardClicked(status_info);
}

void UsersList::setupConnection() {
    connect(ClientSingleton::get_client(), &Client::find_user_result, this, &UsersList::slotFindUserResult);
    connect(ui->searchEdit, &QLineEdit::returnPressed, this, &UsersList::slotsHandleReturnPress);
}
