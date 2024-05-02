#include "channelwidget.h"
#include "ui_channelwidget.h"

#include <QFileDialog>

#include "utility.h"
#include "clientsingleton.h"
#include "infodialog.h"
#include "channelmessage.h"
#include "messagecard.h"
#include <ctime>

ChannelWidget::ChannelWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChannelWidget)
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

    slotClear();
    ClientSingleton::get_client()->async_is_channel_participant_request(channel_info_.id_);
}

void ChannelWidget::slotHandleIsChannelParticipantResult(int result, bool is_participant) {
    if (result == IS_CHANNEL_PARTICIPANT_SUCCESS_ANSWER) {
        if (is_participant) {
            ui->stackedWidget->setCurrentIndex(0); // PARTICIPANT_PAGE
            if (channel_info_.owner_id_ == ClientSingleton::get_client()->get_status_info().id_) { // если текущий пользователь - владелец канала,
                ui->stackedWidget->show();                                                         // отображаем панель для отправки сообщений
            } else {
                ui->stackedWidget->hide();
            }
        } else {
            ui->stackedWidget->show();
            ui->stackedWidget->setCurrentIndex(1); // NOT_PARTICIPANT_PAGE
        }

        ClientSingleton::get_client()->async_get_channel_messages(channel_info_.id_);
    } else {
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Ошибка получения информации об участниках канала. Попытайтесь позже. ");
        dialog_ptr->exec();

        ui->stackedWidget->show();
        ui->stackedWidget->setCurrentIndex(1); // NOT_PARTICIPANT_PAGE
    }
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
                phboxLayout->addWidget(new MessageCard(nullptr, pix));
                phboxLayout->addStretch();

                ui->verticalLayout->addLayout(phboxLayout);
                break;
            }
        }
    } else if (result == SEND_CHNNL_MSG_FAILED_ANSWER){
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Ошибка отправки сообщения в канал. Попытайтесь снова. ");
        dialog_ptr->exec();
    }
}


void ChannelWidget::slotHandleGetMessages(int result) {
    slotClear();

    if (result == GET_CHNNL_MSG_SUCCESS_ANSWER){
        std::vector<std::shared_ptr<IMessage>> messages = ClientSingleton::get_client()->get_messages();
        for (auto& elem : messages) {
            QHBoxLayout *phboxLayout = new QHBoxLayout();
            switch(elem->get_msg_content()->get_msg_content_type()) {
                case TEXT_MSG_TYPE: {
                    phboxLayout->addWidget(new MessageCard(nullptr, elem->get_msg_content()->get_raw_data()));
                    phboxLayout->addStretch();
                    break;
                }
                case IMAGE_MSG_TYPE: {
                    QPixmap pix;
                    pix.load(elem->get_msg_content()->get_raw_data());
                    pix = pix.scaledToWidth(450);

                    phboxLayout->addWidget(new MessageCard(nullptr, pix));
                    phboxLayout->addStretch();

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
            image_path_ = MessageUtility::copy_image_to_ind_folder(str);
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

void ChannelWidget::setupConnection() {
    connect(ui->messageEdit,       &QLineEdit::returnPressed, this, &ChannelWidget::slotSendMessage);
    connect(ui->sendButton,        &QPushButton::clicked,     this, &ChannelWidget::slotSendMessage);
    connect(ui->cameraButton,       &QPushButton::clicked,     this, &ChannelWidget::slotChooseImage);

    connect(ClientSingleton::get_client(), &Client::send_msg_result, this, &ChannelWidget::slotHandleSendMessage);
    connect(ClientSingleton::get_client(), &Client::get_channel_msg_result, this, &ChannelWidget::slotHandleGetMessages);
    connect(ClientSingleton::get_client(), &Client::is_channel_participant_result, this, &ChannelWidget::slotHandleIsChannelParticipantResult);
    connect(ClientSingleton::get_client(), &Client::add_participant_to_channel_result, this, &ChannelWidget::slotHandleAddParticipantChannelResult);
    connect(ui->joinButton, &QPushButton::clicked, ClientSingleton::get_client(), [this] () {
                                                                                    ClientSingleton::get_client()->async_add_channel_participant_request(channel_info_.id_);
                                                                                  });
}
