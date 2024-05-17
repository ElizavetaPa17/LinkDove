#include "channelwidget.h"
#include "ui_channelwidget.h"

#include <QFileDialog>

#include "utility.h"
#include "clientsingleton.h"
#include "infodialog.h"
#include "listlabeldialog.h"
#include "channelmessage.h"
#include "messagecard.h"
#include "agreedialog.h"
#include "typestringdialog.h"
#include "audiomanagersingleton.h"

ChannelWidget::ChannelWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ChannelWidget)
    , audio_dir_(qApp->applicationDirPath() + "/" + MEDIA_CHANNEL_AUDIO_PATH)
{
    ui->setupUi(this);

    setupConnection();
}

ChannelWidget::~ChannelWidget()
{
    delete ui;
}

void ChannelWidget::slotOpenChannel(const ChannelInfo &channel_info) {
    channel_info_ = channel_info;
    ui->infoLabel->setText(channel_info_.name_.c_str());

    slotClear();
    ClientSingleton::get_client()->async_is_channel_participant_request(channel_info_.id_);

    emit openChannelWidget();
}

void ChannelWidget::slotHandleIsChannelParticipantResult(int result, bool is_participant) {
    if (result == IS_CHANNEL_PARTICIPANT_SUCCESS_ANSWER) {
        if (is_participant) {
            ui->stackedWidget->setCurrentIndex(0); // PARTICIPANT_PAGE
            ui->quitButton->show();

            if (channel_info_.owner_id_ == ClientSingleton::get_client()->get_status_info().id_
                || ADMIN_ID == ClientSingleton::get_client()->get_status_info().id_) { // если текущий пользователь - владелец канала,
                ui->stackedWidget->show();                                                         // отображаем панель для отправки сообщений или админ
                ui->deleteButton->show();
                ui->removeUserButton->show();
            } else {
                ui->stackedWidget->hide();
                ui->deleteButton->hide();
                ui->removeUserButton->hide();
            }
        } else {
            ui->stackedWidget->show();
            ui->deleteButton->hide();
            ui->quitButton->hide();
            ui->removeUserButton->hide();
            ui->stackedWidget->setCurrentIndex(1); // NOT_PARTICIPANT_PAGE
        }

    } else {
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Ошибка получения информации об участниках канала. Попытайтесь позже. ");
        dialog_ptr->exec();

        ui->stackedWidget->show();
        ui->stackedWidget->setCurrentIndex(1); // NOT_PARTICIPANT_PAGE
    }

    ClientSingleton::get_client()->async_get_channel_messages(channel_info_.id_);
}

void ChannelWidget::slotClear() {
    ui->messageEdit->clear();
    QtUtility::clean_complex_layout(ui->verticalLayout);
    ui->verticalLayout->addStretch();
}

void ChannelWidget::slotSendMessage() {
    if (!ui->messageEdit->text().isEmpty()) {
        std::shared_ptr<ChannelMessage> ind_message = MessageUtility::create_channel_text_message(channel_info_.id_,
                                                                                                  ui->messageEdit->text().toStdString());
        ClientSingleton::get_client()->async_send_message(*ind_message);
        send_msg_type_ = TEXT_MSG_TYPE;
    }
}

void ChannelWidget::slotHandleSendMessage(int result) {
    if (result == SEND_CHNNL_MSG_SUCCESS_ANSWER) {
        ClientSingleton::get_client()->async_get_channel_messages(channel_info_.id_);
    } else if (result == SEND_CHNNL_MSG_FAILED_ANSWER){
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Ошибка отправки сообщения в канал. Попытайтесь снова. ");
        dialog_ptr->exec();
    }
}


void ChannelWidget::slotHandleGetMessages(int result, std::vector<std::shared_ptr<IMessage>> messages) {
    slotClear();

    unsigned long long owner_id = ClientSingleton::get_client()->get_status_info().id_;

    if (result == GET_CHNNL_MSG_SUCCESS_ANSWER){
        for (auto& elem : messages) {
            QHBoxLayout *phboxLayout = new QHBoxLayout();

            if (channel_info_.owner_id_ == owner_id) {
                phboxLayout->addWidget(new MessageCard(nullptr, elem, true));
            } else {
                phboxLayout->addWidget(new MessageCard(nullptr, elem));
            }

            phboxLayout->addStretch();

            ui->verticalLayout->addLayout(phboxLayout);
        }

        ui->verticalLayout->addStretch();
    } else {
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Что-то пошло не так при получении сообщений. ");
        dialog_ptr->exec();
    }
}

void ChannelWidget::slotHandleAddParticipantChannelResult(int result) {
    if (result == ADD_PARTICIPANT_TO_CHANNEL_SUCCESS_ANSWER) {
        slotHandleIsChannelParticipantResult(IS_CHANNEL_PARTICIPANT_SUCCESS_ANSWER, true); // искусственно генерируем успешный ответ
    } else {
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Ошибка вступления в канал. Попытайтесь позже. ");
        dialog_ptr->exec();
    }
}

void ChannelWidget::slotChooseImage() {
    QString str = QFileDialog::getOpenFileName(nullptr, "Выбор фотографии", "", "*.png *.jpg");

    if (!str.isEmpty()) {
        try {
            image_path_ = MessageUtility::copy_image_to_channel_folder(str);
            std::shared_ptr<ChannelMessage> ind_message = MessageUtility::create_channel_image_message(channel_info_.id_,
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

void ChannelWidget::slotRecordAudio() {
    if (is_recording_) {
        ui->microphoneButton->setIcon(QIcon(":/recources/../resources/microphone_icon.png"));
        is_recording_ = false;

        AudioManagerSingleton::get_manager()->stop_recording();

        std::shared_ptr<ChannelMessage> ind_message = MessageUtility::create_channel_audio_message(channel_info_.id_,
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

void ChannelWidget::slotDeleteChannel() {
    std::unique_ptr<AgreeDialog> dialog_ptr = std::make_unique<AgreeDialog>(nullptr, "Вы точно хотите удалить канал?");
    if (dialog_ptr->exec() == QDialog::Accepted) {
        ClientSingleton::get_client()->async_delete_channel(channel_info_.id_);
    }
}

void ChannelWidget::slotHandleDeleteResult(int result) {
    if (result == DELETE_CHANNEL_SUCCESS_ANSWER) {
        slotClear();
    } else {
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Что-то пошло не так при попытке удалить канал. ");
        dialog_ptr->exec();
    }
}

void ChannelWidget::slotQuitChannel() {
    std::unique_ptr<AgreeDialog> dialog_ptr = std::make_unique<AgreeDialog>(nullptr, "Вы точно хотите покинуть канал?");
    if (dialog_ptr->exec() == QDialog::Accepted) {
        ClientSingleton::get_client()->async_quit_channel(ClientSingleton::get_client()->get_status_info().id_, channel_info_.id_);
    }
}

void ChannelWidget::slotQuitChannelResult(int result) {
    if (result == QUIT_CHANNEL_SUCCESS_ANSWER) {
        slotClear();
    } else {
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Что-то пошло не так при попытке покинуть канал. ");
        dialog_ptr->exec();
    }
}

void ChannelWidget::slotRemoveUser() {
    std::unique_ptr<TypeStringDialog> dialog_ptr = std::make_unique<TypeStringDialog>(nullptr, "Введите никнейм пользователя: ");
    if (dialog_ptr->exec() == QDialog::Accepted) {
        if (dialog_ptr->getString() == ClientSingleton::get_client()->get_status_info().username_) {
            std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Вы не можете удалить себя из канала. ");
            dialog_ptr->exec();
            return;
        }

        ClientSingleton::get_client()->async_remove_user_from_channel(channel_info_.id_, dialog_ptr->getString());
    }
}

void ChannelWidget::slotRemoveUserResult(int result) {
    if (result == REMOVE_USER_FROM_CHANNEL_SUCCESS_ANSWER) {
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Пользователь удален из канала. ");
        dialog_ptr->exec();
    } else {
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Что-то пошло не так при попытке удалить пользователя из канала. ");
        dialog_ptr->exec();
    }
}

void ChannelWidget::slotGetParticipantListResult(int result, std::vector<std::string> participants) {
    if (result == GET_CHNNL_PARTICIPANTS_SUCCESS_ANSWER) {
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

void ChannelWidget::slotDeleteMessageResult(int result) {
    if (result == DEL_CHANNEL_MSG_SUCCESS_ANSWER) {
        ClientSingleton::get_client()->async_get_channel_messages(channel_info_.id_);
    } else if (result == DEL_CHANNEL_MSG_FAILED_ANSWER) {
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Что-то пошло не так при попытке удалить сообщение.");
        dialog_ptr->exec();
    }
}

void ChannelWidget::setupConnection() {
    connect(ui->messageEdit,       &QLineEdit::returnPressed, this, &ChannelWidget::slotSendMessage);
    connect(ui->sendButton,        &QPushButton::clicked,     this, &ChannelWidget::slotSendMessage);
    connect(ui->microphoneButton,  &QPushButton::clicked,     this, &ChannelWidget::slotRecordAudio);
    connect(ui->cameraButton,      &QPushButton::clicked,     this, &ChannelWidget::slotChooseImage);

    connect(ClientSingleton::get_client(), &Client::send_msg_result,                   this, &ChannelWidget::slotHandleSendMessage);
    connect(ClientSingleton::get_client(), &Client::get_channel_msg_result,            this, &ChannelWidget::slotHandleGetMessages);
    connect(ClientSingleton::get_client(), &Client::is_channel_participant_result,     this, &ChannelWidget::slotHandleIsChannelParticipantResult);
    connect(ClientSingleton::get_client(), &Client::add_participant_to_channel_result, this, &ChannelWidget::slotHandleAddParticipantChannelResult);
    connect(ClientSingleton::get_client(), &Client::delete_channel_result,             this, &ChannelWidget::slotHandleDeleteResult);
    connect(ClientSingleton::get_client(), &Client::quit_channel_result,               this, &ChannelWidget::slotQuitChannelResult);
    connect(ClientSingleton::get_client(), &Client::remove_user_from_channel_result,   this, &ChannelWidget::slotRemoveUserResult);
    connect(ClientSingleton::get_client(), &Client::get_channel_participants_result,   this, &ChannelWidget::slotGetParticipantListResult);
    connect(ClientSingleton::get_client(), &Client::delete_msg_result,                 this, &ChannelWidget::slotDeleteMessageResult);

    connect(ui->joinButton, &QPushButton::clicked, ClientSingleton::get_client(), [this] () {
                                                                                    ClientSingleton::get_client()->async_add_channel_participant_request(channel_info_.id_);
                                                                                  });
    connect(ui->deleteButton,       &QPushButton::clicked, this, &ChannelWidget::slotDeleteChannel);
    connect(ui->quitButton,         &QPushButton::clicked, this, &ChannelWidget::slotQuitChannel);
    connect(ui->removeUserButton,   &QPushButton::clicked, this, &ChannelWidget::slotRemoveUser);
    connect(ui->participantsButton, &QPushButton::clicked, this, [this]() {
                                                                            ClientSingleton::get_client()->async_get_channel_participants(channel_info_.id_);
                                                                          });
}
