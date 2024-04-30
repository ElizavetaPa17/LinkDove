#include "channelwidget.h"
#include "ui_channelwidget.h"

#include "utility.h"
#include "clientsingleton.h"
#include "infodialog.h"

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

    ClientSingleton::get_client()->async_is_channel_participant_request(channel_info.id_);
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
    } else {
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Ошибка получения информации об участниках канала. Попытайтесь позже. ");
        dialog_ptr->exec();
    }
}

void ChannelWidget::slotClear() {
    std::cerr << "clear channels widget\n";
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

void ChannelWidget::slotHandleAddParticipandChannelResult(int result) {
    if (result == ADD_PARTICIPANT_TO_CHANNEL_SUCCESS_ANSWER) {
        slotHandleIsChannelParticipantResult(IS_CHANNEL_PARTICIPANT_SUCCESS_ANSWER, true); // искусственно генерируем успешный ответ
    } else {
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Ошибка вступления в канал. Попытайтесь позже. ");
        dialog_ptr->exec();
    }
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
    connect(ClientSingleton::get_client(), &Client::is_channel_participant_result, this, &ChannelWidget::slotHandleIsChannelParticipantResult);
    connect(ClientSingleton::get_client(), &Client::add_participant_to_channel_result, this, &ChannelWidget::slotHandleAddParticipandChannelResult);
    connect(ui->joinButton, &QPushButton::clicked, ClientSingleton::get_client(), [this] () {
                                                                                    ClientSingleton::get_client()->async_add_channel_participant_request(channel_info_.id_);
                                                                                  });
}
