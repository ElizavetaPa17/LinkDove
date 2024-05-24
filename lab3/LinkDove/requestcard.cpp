#include "requestcard.h"
#include "ui_requestcard.h"

#include "clientsingleton.h"

RequestCard::RequestCard(QWidget *parent, const std::string &username) :
    QWidget(parent),
    ui(new Ui::RequestCard),
    username_(username)
{
    ui->setupUi(this);

    ui->usernameLabel->setText(username_.c_str());
    setupConnections();
}

RequestCard::~RequestCard()
{
    delete ui;
}

std::string RequestCard::getUsername() {
    return username_;
}

void RequestCard::setupConnections() {
    connect(ui->addButton,    &QPushButton::clicked, this, [this]() { emit add(username_); });
    connect(ui->removeButton, &QPushButton::clicked, this, [this]() { emit remove(username_); });
}
