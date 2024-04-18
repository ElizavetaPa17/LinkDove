#include "messagecard.h"
#include "ui_messagecard.h"

#include <QStyleOption>
#include <QPainter>

MessageCard::MessageCard(QWidget *parent, QString text) :
    QWidget(parent),
    ui(new Ui::MessageCard)
{
    ui->setupUi(this);
    ui->label->setText(text);
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
