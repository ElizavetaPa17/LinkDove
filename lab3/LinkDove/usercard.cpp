#include "usercard.h"
#include "ui_usercard.h"

#include "clientsingleton.h"

UserCard::UserCard(QWidget *parent, const StatusInfo &status_info)
    : QWidget(parent)
    , ui(new Ui::UserCard)
    , status_info_(status_info)
{
    ui->setupUi(this);
    ui->usernameLabel->setText(status_info.username_.c_str());
}

UserCard::~UserCard()
{
    delete ui;
}

void UserCard::mousePressEvent(QMouseEvent *event) {
    emit userCardClicked(status_info_);
}
