#include "channelwidget.h"
#include "ui_channelwidget.h"

#include "utility.h"
#include "clientsingleton.h"

ChannelWidget::ChannelWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChannelWidget)
{
    ui->setupUi(this);
}

ChannelWidget::~ChannelWidget()
{
    delete ui;
}

void ChannelWidget::slotOpenChannel(const ChannelInfo &channel_info) {
    channel_info_ = channel_info;
}

void ChannelWidget::slotClear() {
    ui->messageEdit->clear();
    QtUtility::clean_complex_layout(ui->verticalLayout);
    ui->verticalLayout->addStretch();
}

void ChannelWidget::slotSendMessage() {

}

void ChannelWidget::slotHandleSendMessage(int result) {

}


void ChannelWidget::slotHandleGetMessages(int result) {

}

void ChannelWidget::slotChooseImage() {

}

void ChannelWidget::setupConnection() {
    connect(ui->messageEdit,       &QLineEdit::returnPressed, this, &ChannelWidget::slotSendMessage);
    connect(ui->sendButton,        &QPushButton::clicked,     this, &ChannelWidget::slotSendMessage);
    connect(ui->cameraButton,       &QPushButton::clicked,     this, &ChannelWidget::slotChooseImage);
    //connect(ui->infoLabel, &ClickableLabel::clicked,  this, &ChannelWidget::slotDisplayInterlocutorProfile);

    connect(ClientSingleton::get_client(), &Client::send_msg_result, this, &ChannelWidget::slotHandleSendMessage);
    connect(ClientSingleton::get_client(), &Client::get_ind_msg_result, this, &ChannelWidget::slotHandleGetMessages);
}
