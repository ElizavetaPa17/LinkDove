#include "channelcard.h"
#include "ui_channelcard.h"

#include <QStyleOption>
#include <QPainter>

#include "clientsingleton.h"

ChannelCard::ChannelCard(QWidget *parent, const ChannelInfo &channel_info)
    : QWidget(parent)
    , ui(new Ui::ChannelCard)
    , channel_info_(channel_info)
{
    ui->setupUi(this);
    ui->channelLabel->setText(channel_info.name_.c_str());
}

ChannelCard::~ChannelCard()
{
    delete ui;
}

void ChannelCard::mousePressEvent(QMouseEvent *event) {
    emit channelCardClicked(channel_info_);
}

void ChannelCard::paintEvent(QPaintEvent *) {
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
