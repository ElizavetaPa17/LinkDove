#include "chatwidget.h"
#include "ui_chatwidget.h"

#include <iostream>
#include <QSpacerItem>
#include <QFileDialog>
#include <QtMultimedia>
#include <QAudioFormat>
#include <QAudio>

#include "infodialog.h"
#include "agreedialog.h"
#include "interlocutorprofiledialog.h"
#include "messagecard.h"
#include "individualmessage.h"
#include "textmessagecontent.h"
#include "imagemessagecontent.h"
#include "clientsingleton.h"
#include "utility.h"

ChatWidget::ChatWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ChatWidget)
    , audio_dir_(qApp->applicationDirPath() + "/" + MEDIA_IND_AUDIO_PATH)
{
    ui->setupUi(this);

    setupConnection();
}

ChatWidget::~ChatWidget() {
    delete ui;
}

void ChatWidget::slotOpenChatWith(const StatusInfo &status_info) {
    interlocutor_ = status_info;
    ui->infoLabel->setText(status_info.username_.c_str());

    slotClear();
    ClientSingleton::get_client()->async_get_ind_messages(interlocutor_.id_);

    emit openChatWidget();
}

void ChatWidget::slotSendMessage() {
    if (!ui->messageEdit->text().isEmpty()) {
        std::shared_ptr<IndividualMessage> ind_message = MessageUtility::create_individual_text_message(ClientSingleton::get_client()->get_status_info().id_,
                                                                                                        interlocutor_.id_,
                                                                                                        ui->messageEdit->text().toStdString());
        ClientSingleton::get_client()->async_send_message(*ind_message);
        send_msg_type_ = TEXT_MSG_TYPE;
    }
}

// ПОВТОР КОДА С КОДОМ ИЗ CHAT_WIDGET! ПЕРЕДЕЛАТЬ!
void ChatWidget::slotHandleSendMessage(int result) {
    if (result == SEND_IND_MSG_SUCCESS_ANSWER) {
        switch (send_msg_type_) {
            case TEXT_MSG_TYPE: {
                QHBoxLayout *phboxLayout = new QHBoxLayout();
                phboxLayout->addStretch();
                phboxLayout->addWidget(new MessageCard(nullptr, ui->messageEdit->text()));

                ui->verticalLayout->addLayout(phboxLayout);
                ui->verticalLayout->addStretch();
                ui->messageEdit->setText("");
                break;
            }
            case IMAGE_MSG_TYPE: {
                QPixmap pix;
                pix.load(image_path_.c_str());
                pix = pix.scaledToWidth(450);

                QHBoxLayout *phboxLayout = new QHBoxLayout();
                phboxLayout->addStretch();
                phboxLayout->addWidget(new MessageCard(nullptr, pix, image_path_.c_str()));

                ui->verticalLayout->addLayout(phboxLayout);
                break;
            }
        }
    } else if (result == SEND_IND_MSG_FAILED_ANSWER){
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Ошибка отправки сообщения собеседнику. Попытайтесь снова. ");
        dialog_ptr->exec();
    }
}

void ChatWidget::slotHandleGetMessages(int result, std::vector<std::shared_ptr<IMessage>> messages) {
    if (result == GET_IND_MSG_SUCCESS_ANSWER){
        for (auto& elem : messages) {
            QHBoxLayout *phboxLayout = new QHBoxLayout();
            switch(elem->get_msg_content()->get_msg_content_type()) {
                case TEXT_MSG_TYPE: {
                    if (std::dynamic_pointer_cast<IndividualMessage>(elem)->get_msg_edges().second == interlocutor_.id_) { // убрать dynamic_cast, расширив базовый класс!!!! TODO!!!!
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
                    pix = pix.scaledToWidth(450);

                    if (std::dynamic_pointer_cast<IndividualMessage>(elem)->get_msg_edges().second == interlocutor_.id_) { // убрать dynamic_cast, расширив базовый класс!!!! TODO!!!!
                        phboxLayout->addStretch();
                        phboxLayout->addWidget(new MessageCard(nullptr, pix, elem->get_msg_content()->get_raw_data()));
                    } else {
                        phboxLayout->addWidget(new MessageCard(nullptr, pix, elem->get_msg_content()->get_raw_data()));
                        phboxLayout->addStretch();
                    }

                    break;
                }
            }

            ui->verticalLayout->addLayout(phboxLayout);
        }

        ui->verticalLayout->addStretch();
    } else if (result == GET_IND_MSG_FAILED_ANSWER) {
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Что-то пошло не так при получении сообщений. ");
        dialog_ptr->exec();
    }
}

void ChatWidget::slotChooseImage() {
    QString str = QFileDialog::getOpenFileName(nullptr, "Выбор фотографии", "", "*.png *.jpg");

    if (!str.isEmpty()) {
        try {
            image_path_ = MessageUtility::copy_image_to_ind_folder(str);
            std::shared_ptr<IndividualMessage> ind_message = MessageUtility::create_individual_image_message(ClientSingleton::get_client()->get_status_info().id_,
                                                                                                             interlocutor_.id_,
                                                                                                             image_path_);
            ClientSingleton::get_client()->async_send_message(*ind_message);
            send_msg_type_ = IMAGE_MSG_TYPE;

        } catch (std::runtime_error& ex) {
            std::cerr << ex.what() << '\n';

            std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Что-то пошло не так при попытке отправить сообщение. ");
            dialog_ptr->exec();
        }
    }
}

void ChatWidget::slotRecordAudio() {    
    if (is_recording_) {
        ui->microphoneButton->setIcon(QIcon(":/recources/../resources/microphone_icon.png"));
        is_recording_ = false;

        audio_manager_.stop_recording();
    } else {
        audio_file_ = audio_dir_ + QtUtility::get_random_string(20);
        audio_manager_.start_recording(audio_file_);

        ui->microphoneButton->setIcon(QIcon(":/recources/../resources/record_icon.png"));
        is_recording_ = true;
    }
}

void ChatWidget::slotDisplayInterlocutorProfile() {
    if (!ui->infoLabel->text().isEmpty()) {
        std::unique_ptr<InterlocutorProfileDialog> dialog_ptr = std::make_unique<InterlocutorProfileDialog>(nullptr, interlocutor_);
        dialog_ptr->exec();
    }
}

void ChatWidget::slotDeleteChat() {
    std::unique_ptr<AgreeDialog> dialog_ptr = std::make_unique<AgreeDialog>(nullptr, "Вы точно хотите удалить переписку?");
    if (dialog_ptr->exec() == QDialog::Accepted) {
        ClientSingleton::get_client()->async_delete_ind_chat(interlocutor_.id_);
    }
}

void ChatWidget::slotHandleDeleteResult(int result) {
    if (result == DELETE_IND_CHAT_SUCCESS_ANSWER) {
        slotClear();
    } else {
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Что-то пошло не так при попытке удалить переписку. ");
        dialog_ptr->exec();
    }
}

void ChatWidget::slotClear() {
    ui->messageEdit->clear();
    QtUtility::clean_complex_layout(ui->verticalLayout);
    ui->verticalLayout->addStretch();
}

void ChatWidget::setupConnection() {
    connect(ui->messageEdit,       &QLineEdit::returnPressed, this, &ChatWidget::slotSendMessage);
    connect(ui->sendButton,        &QPushButton::clicked,     this, &ChatWidget::slotSendMessage);
    connect(ui->cameraButton,      &QPushButton::clicked,     this, &ChatWidget::slotChooseImage);
    connect(ui->microphoneButton,  &QPushButton::clicked,     this, &ChatWidget::slotRecordAudio);
    connect(ui->infoLabel,         &ClickableLabel::clicked,  this, &ChatWidget::slotDisplayInterlocutorProfile);

    connect(ClientSingleton::get_client(), &Client::send_msg_result, this, &ChatWidget::slotHandleSendMessage);
    connect(ClientSingleton::get_client(), &Client::get_ind_msg_result, this, &ChatWidget::slotHandleGetMessages);
    connect(ClientSingleton::get_client(), &Client::delete_ind_chat_result, this, &ChatWidget::slotHandleDeleteResult);

    connect(ui->deleteButton, &QPushButton::clicked, this, &ChatWidget::slotDeleteChat);
}
