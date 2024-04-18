#include "chatwidget.h"
#include "ui_chatwidget.h"

#include <iostream>

#include "infodialog.h"
#include "messagecard.h"
#include "individualmessage.h"
#include "textmessagecontent.h"
#include "clientsingleton.h"

ChatWidget::ChatWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ChatWidget)
    , receiver_id_(0)
{
    ui->setupUi(this);

    setupConnection();
}

ChatWidget::~ChatWidget() {
    delete ui;
}

void ChatWidget::slotOpenChatWith(const StatusInfo &status_info) {
    receiver_id_ = status_info.id_;
    ui->label->setText(status_info.username_.c_str());
}

void ChatWidget::slotSendMessage() {
    if (!ui->messageEdit->text().isEmpty()) {
        IndividualMessage ind_message(CREATED_MSG_ID);
        ind_message.set_msg_edges(ClientSingleton::get_client()->get_status_info().id_, receiver_id_);

        std::shared_ptr<TextMessageContent> text_msg_content_ptr = std::make_shared<TextMessageContent>();
        text_msg_content_ptr->set_text(ui->messageEdit->text().toStdString());
        ind_message.set_msg_content(text_msg_content_ptr);

        ClientSingleton::get_client()->async_send_message(ind_message);
    }
}

void ChatWidget::slotHandleSendMessage(int result) {
    if (result == SEND_MSG_SUCCESS_ANSWER) {
        ui->rightVerticalLayout->addWidget(new MessageCard(nullptr, ui->messageEdit->text()));
        ui->messageEdit->setText("");
    } else {
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>("Ошибка отправки сообщения. Попытайтесь снова. ");
        dialog_ptr->exec();
    }

    std::cerr << "here\n";
}

void ChatWidget::setupConnection() {
    connect(ui->messageEdit, &QLineEdit::returnPressed, this, &ChatWidget::slotSendMessage);
    connect(ui->sendButton, &QPushButton::clicked, this, &ChatWidget::slotSendMessage);
    connect(ClientSingleton::get_client(), &Client::send_msg_result, this, &ChatWidget::slotHandleSendMessage);
}
