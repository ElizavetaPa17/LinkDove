#include "chatcard.h"
#include "ui_chatcard.h"

#include <QStyleOption>
#include <QPainter>

ChatCard::ChatCard(QWidget *parent, const ChatInfo &chat_info)
    : QWidget(parent)
    , ui(new Ui::ChatCard)
    , chat_info_(chat_info)
{
    ui->setupUi(this);
    ui->chatLabel->setText(chat_info_.name_.c_str());
}

ChatCard::~ChatCard()
{
    delete ui;
}

void ChatCard::mousePressEvent(QMouseEvent *event) {
    emit chatCardClicked(chat_info_);
}

void ChatCard::paintEvent(QPaintEvent *) {
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
