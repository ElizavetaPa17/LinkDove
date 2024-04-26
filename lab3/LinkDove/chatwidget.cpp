#include "chatwidget.h"
#include "ui_chatwidget.h"

#include <iostream>
#include <QSpacerItem>
#include <QFileDialog>

#include "infodialog.h"
#include "messagecard.h"
#include "individualmessage.h"
#include "textmessagecontent.h"
#include "imagemessagecontent.h"
#include "clientsingleton.h"
#include "utility.h"

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

    slotClear();
    ClientSingleton::get_client()->async_get_ind_messages(receiver_id_);
}

void ChatWidget::slotSendMessage() {
    if (!ui->messageEdit->text().isEmpty()) {
        std::shared_ptr<IndividualMessage> ind_message = MessageUtility::create_individual_text_message(ClientSingleton::get_client()->get_status_info().id_,
                                                                                                        receiver_id_,
                                                                                                        ui->messageEdit->text().toStdString());
        ClientSingleton::get_client()->async_send_message(*ind_message);
        send_msg_type_ = TEXT_MSG_TYPE;
    }
}

void ChatWidget::slotHandleSendMessage(int result) {
    if (result == SEND_MSG_SUCCESS_ANSWER) {
        switch (send_msg_type_) {
            case TEXT_MSG_TYPE: {
                QHBoxLayout *phboxLayout = new QHBoxLayout();
                phboxLayout->addStretch();
                phboxLayout->addWidget(new MessageCard(nullptr, ui->messageEdit->text()));

                ui->verticalLayout->addLayout(phboxLayout);
                ui->verticalLayout->addStretch();
                ui->messageEdit->setText("");
            }
            case IMAGE_MSG_TYPE: {
                QPixmap pix;
                pix.load(image_path_.c_str());
                pix = pix.scaledToWidth(450);

                QHBoxLayout *phboxLayout = new QHBoxLayout();
                phboxLayout->addStretch();
                phboxLayout->addWidget(new MessageCard(nullptr, pix));

                ui->verticalLayout->addLayout(phboxLayout);
            }
        }
    } else {
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>("Ошибка отправки сообщения. Попытайтесь снова. ");
        dialog_ptr->exec();
    }
}

void ChatWidget::slotHandleGetMessages(int result) {
    if (result == GET_IND_MSG_SUCCESS_ANSWER){
        std::vector<std::shared_ptr<IMessage>> messages = ClientSingleton::get_client()->get_messages();
        for (auto& elem : messages) {
            QHBoxLayout *phboxLayout = new QHBoxLayout();
            switch(elem->get_msg_content()->get_msg_content_type()) {
                case TEXT_MSG_TYPE: {
                    if (std::dynamic_pointer_cast<IndividualMessage>(elem)->get_msg_edges().second == receiver_id_) { // убрать dynamic_cast, расширив базовый класс!!!! TODO!!!!
                        phboxLayout->addStretch();
                        phboxLayout->addWidget(new MessageCard(nullptr, elem->get_msg_content()->get_raw_data()));
                    } else {
                        phboxLayout->addWidget(new MessageCard(nullptr, elem->get_msg_content()->get_raw_data()));
                        phboxLayout->addStretch();
                    }

                    break;
                }
                case IMAGE_MSG_TYPE: {
                    QPixmap pix;
                    pix.load(elem->get_msg_content()->get_raw_data());
                    std::cerr << elem->get_msg_content()->get_raw_data() << '\n' << pix.isNull() << '\n';
                    pix = pix.scaledToWidth(450);

                    if (std::dynamic_pointer_cast<IndividualMessage>(elem)->get_msg_edges().second == receiver_id_) { // убрать dynamic_cast, расширив базовый класс!!!! TODO!!!!
                        phboxLayout->addStretch();
                        std::cerr << "one\n";
                        phboxLayout->addWidget(new MessageCard(nullptr, pix));
                    } else {
                        std::cerr << "two\n";
                        phboxLayout->addWidget(new MessageCard(nullptr, pix));
                        phboxLayout->addStretch();
                    }

                    break;
                }
            }

            ui->verticalLayout->addLayout(phboxLayout);
        }

        ui->verticalLayout->addStretch();
    } else {
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>("Что-то пошло не так при получении сообщений. ");
        dialog_ptr->exec();
    }
}

void ChatWidget::slotChooseImage() {
    QString str = QFileDialog::getOpenFileName(nullptr, "Выбор фотографии", "", "*.png *.jpg");

    if (!str.isEmpty()) {
        try {
            image_path_ = MessageUtility::copy_image_to_ind_folder(str);
            std::shared_ptr<IndividualMessage> ind_message = MessageUtility::create_individual_image_message(ClientSingleton::get_client()->get_status_info().id_,
                                                                                                             receiver_id_,
                                                                                                             image_path_);
            ClientSingleton::get_client()->async_send_message(*ind_message);
            send_msg_type_ = IMAGE_MSG_TYPE;

        } catch (std::runtime_error& ex) {
            std::cerr << ex.what() << '\n';

            std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>("Что-то пошло не так при попытке отправить сообщение. ");
            dialog_ptr->exec();
            return;
        }
    }
}

void ChatWidget::slotClear() {
    ui->messageEdit->clear();
    QtUtility::clean_complex_layout(ui->verticalLayout);
    ui->verticalLayout->addStretch();
}

void ChatWidget::setupConnection() {
    connect(ui->messageEdit, &QLineEdit::returnPressed, this, &ChatWidget::slotSendMessage);
    connect(ui->sendButton, &QPushButton::clicked, this, &ChatWidget::slotSendMessage);
    connect(ui->mediaButton, &QPushButton::clicked, this, &ChatWidget::slotChooseImage);
    connect(ClientSingleton::get_client(), &Client::send_msg_result, this, &ChatWidget::slotHandleSendMessage);
    connect(ClientSingleton::get_client(), &Client::get_ind_msg_result, this, &ChatWidget::slotHandleGetMessages);
}
