#include "messagecard.h"
#include "ui_messagecard.h"

#include <QStyleOption>
#include <QPainter>

MessageCard::MessageCard(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MessageCard)
{

}

MessageCard::MessageCard(QWidget *parent, QString text)
    : MessageCard(parent)
{
    ui->setupUi(this);
    ui->label->setText(text);
}

MessageCard::MessageCard(QWidget *parent, const QPixmap& photo)
    : MessageCard(parent)
{
    ui->setupUi(this);
    ui->label->setPixmap(photo);
}

MessageCard::~MessageCard()
{
    delete ui;
}

void MessageCard::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
