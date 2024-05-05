#include "groupwidget.h"
#include "ui_groupwidget.h"

#include <QFileDialog>

#include "groupmessage.h"
#include "clientsingleton.h"
#include "infodialog.h"
#include "messagecard.h"
#include "utility.h"

GroupWidget::GroupWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GroupWidget)
{
    ui->setupUi(this);

    setupConnection();
}

GroupWidget::~GroupWidget()
{
    delete ui;
}

void GroupWidget::slotOpenGroup(const ChatInfo &group_info) {
    chat_info_ = group_info;
    ui->infoLabel->setText(group_info.name_.c_str());

    slotClear();
    ClientSingleton::get_client()->async_is_chat_participant_request(group_info.id_);
}

void GroupWidget::slotHandleIsGroupParticipantResult(int result, bool is_participant) {
    if (result == IS_CHAT_PARTICIPANT_SUCCESS_ANSWER) {
        if (is_participant) {
            ui->stackedWidget->setCurrentIndex(0); // PARTICIPANT_PAGE
        } else {
            ui->stackedWidget->setCurrentIndex(1); // NOT_PARTICIPANT_PAGE
        }
    } else {
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Ошибка получения информации об участниках группы. Попытайтесь позже. ");
        dialog_ptr->exec();

        ui->stackedWidget->setCurrentIndex(1); // NOT_PARTICIPANT_PAGE
    }

    ClientSingleton::get_client()->async_get_chat_messages(chat_info_.id_);
}

void GroupWidget::slotClear() {
    ui->messageEdit->clear();
    QtUtility::clean_complex_layout(ui->verticalLayout);
    ui->verticalLayout->addStretch();
}

void GroupWidget::slotSendMessage() {
    if (!ui->messageEdit->text().isEmpty()) {
        std::cerr << chat_info_.id_ << '\n';
        std::shared_ptr<GroupMessage> message = MessageUtility::create_group_text_message(chat_info_.id_,
                                                                                          ClientSingleton::get_client()->get_status_info().id_,
                                                                                          ui->messageEdit->text().toStdString());
        ClientSingleton::get_client()->async_send_message(*message);
        send_msg_type_ = TEXT_MSG_TYPE;
    }
}

void GroupWidget::slotHandleSendMessage(int result) {
    if (result == SEND_CHAT_MSG_SUCCESS_ANSWER) {
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
                ui->verticalLayout->addStretch();
                break;
            }
        }
    } else if (result == SEND_CHAT_MSG_FAILED_ANSWER){
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Ошибка отправки сообщения в группу. Попытайтесь снова. ");
        dialog_ptr->exec();
    }
}

void GroupWidget::slotHandleGetMessages(int result) {
    slotClear();

    if (result == GET_CHAT_MSG_SUCCESS_ANSWER){
        std::vector<std::shared_ptr<IMessage>> messages = ClientSingleton::get_client()->get_messages();
        unsigned long long user_id = ClientSingleton::get_client()->get_status_info().id_;
        for (auto& elem : messages) {
            QHBoxLayout *phboxLayout = new QHBoxLayout();
            switch(elem->get_msg_content()->get_msg_content_type()) {
                case TEXT_MSG_TYPE: {
                    std::cerr << std::dynamic_pointer_cast<GroupMessage>(elem)->get_owner_id() << '\n';
                    if (std::dynamic_pointer_cast<GroupMessage>(elem)->get_owner_id() == user_id) {
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

                    if (std::dynamic_pointer_cast<GroupMessage>(elem)->get_owner_id() == user_id) {
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
    } else {
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Что-то пошло не так при получении сообщений. ");
        dialog_ptr->exec();
    }
}

void GroupWidget::slotHandleAddParticipantGroupResult(int result) {
    if (result == ADD_PARTICIPANT_TO_CHAT_SUCCESS_ANSWER) {
        slotHandleIsGroupParticipantResult(IS_CHAT_PARTICIPANT_SUCCESS_ANSWER, true); // искусственно генерируем успешный ответ
    } else {
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Ошибка вступления в группу. Попытайтесь позже. ");
        dialog_ptr->exec();
    }
}

void GroupWidget::slotChooseImage() {
    QString str = QFileDialog::getOpenFileName(nullptr, "Выбор фотографии", "", "*.png *.jpg");

    if (!str.isEmpty()) {
        try {
            image_path_ = MessageUtility::copy_image_to_chat_folder(str);
            std::shared_ptr<GroupMessage> ind_message = MessageUtility::create_group_image_message(chat_info_.id_,
                                                                                                     ClientSingleton::get_client()->get_status_info().id_,
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

void GroupWidget::setupConnection() {
    connect(ui->messageEdit,       &QLineEdit::returnPressed, this, &GroupWidget::slotSendMessage);
    connect(ui->sendButton,        &QPushButton::clicked,     this, &GroupWidget::slotSendMessage);
    connect(ui->cameraButton,       &QPushButton::clicked,     this, &GroupWidget::slotChooseImage);

    connect(ClientSingleton::get_client(), &Client::send_msg_result, this, &GroupWidget::slotHandleSendMessage);
    connect(ClientSingleton::get_client(), &Client::get_chat_msg_result, this, &GroupWidget::slotHandleGetMessages);
    connect(ClientSingleton::get_client(), &Client::is_chat_participant_result, this, &GroupWidget::slotHandleIsGroupParticipantResult);
    connect(ClientSingleton::get_client(), &Client::add_participant_to_chat_result, this, &GroupWidget::slotHandleAddParticipantGroupResult);
    connect(ui->joinButton, &QPushButton::clicked, ClientSingleton::get_client(), [this] () {
                                                                                    ClientSingleton::get_client()->async_add_chat_participant_request(chat_info_.id_);
                                                                                  });
}
