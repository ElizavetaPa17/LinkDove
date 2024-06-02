#include "groupwidget.h"
#include "ui_groupwidget.h"

#include <QFileDialog>

#include "groupmessage.h"
#include "clientsingleton.h"
#include "bandialog.h"
#include "infodialog.h"
#include "agreedialog.h"
#include "typestringdialog.h"
#include "listlabeldialog.h"
#include "messagecard.h"
#include "utility.h"
#include "audiomanagersingleton.h"

GroupWidget::GroupWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::GroupWidget)
    , audio_dir_(qApp->applicationDirPath() + "/" + MEDIA_CHANNEL_AUDIO_PATH)
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

    emit openGroupWidget();
}

void GroupWidget::slotHandleIsGroupParticipantResult(int result, bool is_participant) {
    if (result == IS_CHAT_PARTICIPANT_SUCCESS_ANSWER) {
        if (is_participant) {
            ui->stackedWidget->setCurrentIndex(0); // PARTICIPANT_PAGE
            ui->quitButton->show();

            if (chat_info_.owner_id_ == ClientSingleton::get_client()->get_status_info().id_
                || ADMIN_ID == ClientSingleton::get_client()->get_status_info().id_) {
                ui->deleteButton->show();
                ui->removeUserButton->show();
                ui->banButton->show();

                if (chat_info_.is_private_) {
                    ui->requestButton->show();
                } else {
                    ui->requestButton->hide();
                }
            } else {
                ui->removeUserButton->hide();
                ui->deleteButton->hide();
                ui->banButton->hide();
                ui->requestButton->hide();
            }

        } else {
            ui->stackedWidget->setCurrentIndex(1); // NOT_PARTICIPANT_PAGE
            ui->deleteButton->hide();
            ui->quitButton->hide();
            ui->removeUserButton->hide();
            ui->banButton->hide();
            ui->requestButton->hide();
        }

        if (!chat_info_.is_private_ || is_participant) {
            ClientSingleton::get_client()->async_get_chat_messages(chat_info_.id_);
        }
    } else {
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Ошибка получения информации об участниках группы. Попытайтесь позже. ");
        dialog_ptr->exec();

        ui->stackedWidget->setCurrentIndex(1); // NOT_PARTICIPANT_PAGE
        ui->deleteButton->hide();
        ui->quitButton->hide();
        ui->removeUserButton->hide();
        ui->banButton->hide();
    }

    request_dialog_.setBroadChatInfo(chat_info_.id_, false);
}

void GroupWidget::slotClear() {
    ui->messageEdit->clear();
    QtUtility::clean_complex_layout(ui->verticalLayout);
    ui->verticalLayout->addStretch();
}

void GroupWidget::slotSendMessage() {
    if (!ui->messageEdit->text().isEmpty()) {
        std::shared_ptr<GroupMessage> message = MessageUtility::create_group_text_message(chat_info_.id_,
                                                                                          ClientSingleton::get_client()->get_status_info().id_,
                                                                                          ui->messageEdit->text().toStdString());
        ClientSingleton::get_client()->async_send_message(*message);
        send_msg_type_ = TEXT_MSG_TYPE;
    }
}

void GroupWidget::slotHandleSendMessage(int result) {
    if (result == SEND_CHAT_MSG_SUCCESS_ANSWER) {
        ClientSingleton::get_client()->async_get_chat_messages(chat_info_.id_);
    } else if (result == SEND_CHAT_MSG_FAILED_ANSWER){
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Ошибка отправки сообщения в группу. Попытайтесь снова. ");
        dialog_ptr->exec();
    }
}

void GroupWidget::slotHandleGetMessages(int result, std::vector<std::shared_ptr<IMessage>> messages) {
    slotClear();

    if (result == GET_CHAT_MSG_SUCCESS_ANSWER){
        unsigned long long user_id = ClientSingleton::get_client()->get_status_info().id_;
        std::string user_name;

        bool delete_flag = ((user_id == ADMIN_ID) || (user_id == chat_info_.owner_id_)) ? true : false; // Владелец или админ могут удалять любые сообщения

        for (auto& elem : messages) {
            QHBoxLayout *phboxLayout = new QHBoxLayout();

            if (std::dynamic_pointer_cast<GroupMessage>(elem)->get_owner_id() == user_id) {
                phboxLayout->addStretch();
                phboxLayout->addWidget(new MessageCard(nullptr, elem, true));
            } else {
                phboxLayout->addWidget(new MessageCard(nullptr, elem, delete_flag));
                phboxLayout->addStretch();
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

void GroupWidget::slotDeleteGroup() {
    std::unique_ptr<AgreeDialog> dialog_ptr = std::make_unique<AgreeDialog>(nullptr, "Вы точно хотите удалить группу?");
    if (dialog_ptr->exec() == QDialog::Accepted) {
        ClientSingleton::get_client()->async_delete_chat(chat_info_.id_);
    }
}

void GroupWidget::slotRecordAudio() {
    if (is_recording_) {
        ui->microphoneButton->setIcon(QIcon(":/recources/../resources/microphone_icon.png"));
        is_recording_ = false;

        AudioManagerSingleton::get_manager()->stop_recording();

        std::shared_ptr<GroupMessage> ind_message = MessageUtility::create_group_audio_message(chat_info_.id_,
                                                                                                 ClientSingleton::get_client()->get_status_info().id_,
                                                                                                 audio_file_.toStdString() + ".m4a");
        ClientSingleton::get_client()->async_send_message(*ind_message);
        send_msg_type_ = AUDIO_MSG_TYPE;

    } else {
        ui->microphoneButton->setIcon(QIcon(":/recources/../resources/record_icon.png"));
        is_recording_ = true;

        audio_file_ = audio_dir_ + QtUtility::get_random_string(20);
        AudioManagerSingleton::get_manager()->start_recording(audio_file_);
    }
}


void GroupWidget::slotHandleDeleteResult(int result) {
    if (result == DELETE_CHAT_SUCCESS_ANSWER) {
        slotClear();
    } else {
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Что-то пошло не так при попытке удалить группу. ");
        dialog_ptr->exec();
    }
}

void GroupWidget::slotQuitGroup() {
    std::unique_ptr<AgreeDialog> dialog_ptr = std::make_unique<AgreeDialog>(nullptr, "Вы точно хотите покинуть группу?");
    if (dialog_ptr->exec() == QDialog::Accepted) {
        ClientSingleton::get_client()->async_quit_chat(ClientSingleton::get_client()->get_status_info().id_, chat_info_.id_);
    }
}

void GroupWidget::slotQuitGroupResult(int result) {
    if (result == QUIT_CHAT_SUCCESS_ANSWER) {
        slotClear();
    } else {
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Что-то пошло не так при попытке покинуть группу. ");
        dialog_ptr->exec();
    }
}

void GroupWidget::slotRemoveUser() {
    std::unique_ptr<TypeStringDialog> dialog_ptr = std::make_unique<TypeStringDialog>(nullptr, "Введите никнейм пользователя: ");
    if (dialog_ptr->exec() == QDialog::Accepted) {
        if (dialog_ptr->getString() == ClientSingleton::get_client()->get_status_info().username_) {
            std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Вы не можете удалить себя из группы. ");
            dialog_ptr->exec();
            return;
        }

        ClientSingleton::get_client()->async_remove_user_from_chat(chat_info_.id_, dialog_ptr->getString());
    }
}

void GroupWidget::slotBanUser() {
    std::unique_ptr<BanDialog> dialog_ptr = std::make_unique<BanDialog>();
    if (dialog_ptr->exec() == QDialog::Accepted) {
        std::pair<QString, bool> pair = dialog_ptr->get_info();
        if (pair.first.toStdString() == ClientSingleton::get_client()->get_status_info().username_) {
            std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Вы не можете поменять свой статус блокировки. ");
            dialog_ptr->exec();
            return;
        }

        ClientSingleton::get_client()->async_ban_chat_user(pair.first.toStdString(), chat_info_.id_, pair.second);
    }
}

void GroupWidget::slotBanUserResult(int result) {
    std::string text;
    if (result == BAN_CHAT_USER_SUCCESS_ANSWER) {
        text = "Статус блокировки пользователя был изменен.  ";
    } else if (result == BAN_CHAT_USER_FAILED_ANSWER) {
        text = "Что-то пошло не так при попытке изменить статус блокировки пользователя.";
    } else {
        return;
    }

    std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, text);
    dialog_ptr->exec();
}

void GroupWidget::slotRemoveUserResult(int result) {
    if (result == REMOVE_USER_FROM_CHAT_SUCCESS_ANSWER) {
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Пользователь удален из группы. ");
        dialog_ptr->exec();
    } else {
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Что-то пошло не так при попытке удалить пользователя из группы. ");
        dialog_ptr->exec();
    }
}

void GroupWidget::slotGetParticipantListResult(int result, std::vector<std::string> participants) {
    if (result == GET_CHAT_PARTICIPANTS_SUCCESS_ANSWER) {
        if (participants.size() == 0) {
            std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Список участников пуст.");
            dialog_ptr->exec();
        } else {
            std::unique_ptr<ListLabelDialog> dialog_ptr = std::make_unique<ListLabelDialog>(nullptr, participants);
            dialog_ptr->exec();
        }
    } else {
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Что-то пошло не так при попытке получить список участников канала.");
        dialog_ptr->exec();
    }
}

void GroupWidget::slotDeleteMessageResult(int result) {
    if (result == DEL_CHAT_MSG_SUCCESS_ANSWER) {
        ClientSingleton::get_client()->async_get_chat_messages(chat_info_.id_);
    } else if (result == DEL_CHAT_MSG_FAILED_ANSWER) {
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Что-то пошло не так при попытке удалить сообщение.");
        dialog_ptr->exec();
    }
}


void GroupWidget::slotHandleRequestChat(int result) {
    std::string text;

    if (result == REQUEST_PARTICIPANT_TO_CHAT_SUCCESS_ANSWER) {
        text = "Запрос на вступление в группу был отправлен.";
    } else if (result == REQUEST_PARTICIPANT_TO_CHAT_FAILED_ANSWER){
        text = "Что-то пошло не так при попытке отправить запрос на вступление в группу.";
    }

    std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, text);
    dialog_ptr->exec();
}

void GroupWidget::slotGetChatRequestResult(int result, std::vector<std::string> requests) {
    if (result == GET_CHAT_REQUESTS_FAILED_ANSWER) {
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Что-то пошло не так при попытке получить список запросов.");
        dialog_ptr->exec();
        return;
    }

    request_dialog_.removeAllRequests();
    size_t sz = requests.size();

    if (sz == 0) {
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Список запросов пуст.");
        dialog_ptr->exec();
    } else {
        for (int i = 0; i < sz; ++i) {
            request_dialog_.addRequestUsername(requests[i]);
        }

        request_dialog_.exec();
    }
}

void GroupWidget::setupConnection() {
    connect(ui->messageEdit,       &QLineEdit::returnPressed, this, &GroupWidget::slotSendMessage);
    connect(ui->sendButton,        &QPushButton::clicked,     this, &GroupWidget::slotSendMessage);
    connect(ui->cameraButton,      &QPushButton::clicked,     this, &GroupWidget::slotChooseImage);
    connect(ui->microphoneButton,  &QPushButton::clicked,     this, &GroupWidget::slotRecordAudio);

    connect(ClientSingleton::get_client(), &Client::send_msg_result,                this, &GroupWidget::slotHandleSendMessage);
    connect(ClientSingleton::get_client(), &Client::get_chat_msg_result,            this, &GroupWidget::slotHandleGetMessages);
    connect(ClientSingleton::get_client(), &Client::is_chat_participant_result,     this, &GroupWidget::slotHandleIsGroupParticipantResult);
    connect(ClientSingleton::get_client(), &Client::add_participant_to_chat_result, this, &GroupWidget::slotHandleAddParticipantGroupResult);
    connect(ClientSingleton::get_client(), &Client::delete_chat_result,             this, &GroupWidget::slotHandleDeleteResult);
    connect(ClientSingleton::get_client(), &Client::quit_chat_result,               this, &GroupWidget::slotQuitGroupResult);
    connect(ClientSingleton::get_client(), &Client::remove_user_from_chat_result,   this, &GroupWidget::slotRemoveUserResult);
    connect(ClientSingleton::get_client(), &Client::get_chat_participants_result,   this, &GroupWidget::slotGetParticipantListResult);
    connect(ClientSingleton::get_client(), &Client::delete_msg_result,              this, &GroupWidget::slotDeleteMessageResult);
    connect(ClientSingleton::get_client(), &Client::ban_user_result,                this, &GroupWidget::slotBanUserResult);
    connect(ClientSingleton::get_client(), &Client::request_participant_to_chat_result, this, &GroupWidget::slotHandleRequestChat);
    connect(ClientSingleton::get_client(), &Client::get_chat_requests_result,       this, &GroupWidget::slotGetChatRequestResult);

    connect(ui->joinButton, &QPushButton::clicked, ClientSingleton::get_client(), [this] () {
                                                                                    if (!chat_info_.is_private_ || ADMIN_ID == ClientSingleton::get_client()->get_status_info().id_) {
                                                                                        ClientSingleton::get_client()->async_add_chat_participant_request(chat_info_.id_);
                                                                                    } else {
                                                                                        ClientSingleton::get_client()->async_request_chat_participant_request(chat_info_.id_);
                                                                                    }
                                                                                  });

    connect(ui->requestButton,      &QPushButton::clicked, this, [this] () {
                                                                            ClientSingleton::get_client()->async_get_chat_requests_request(chat_info_.id_);
                                                                          });
    connect(ui->deleteButton,       &QPushButton::clicked, this, &GroupWidget::slotDeleteGroup);
    connect(ui->quitButton,         &QPushButton::clicked, this, &GroupWidget::slotQuitGroup);
    connect(ui->removeUserButton,   &QPushButton::clicked, this, &GroupWidget::slotRemoveUser);
    connect(ui->banButton,          &QPushButton::clicked, this, &GroupWidget::slotBanUser);
    connect(ui->participantsButton, &QPushButton::clicked, this, [this]() {
                                                                            ClientSingleton::get_client()->async_get_chat_participants(chat_info_.id_);
                                                                          });
}
