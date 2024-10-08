#include "channellist.h"
#include "ui_channellist.h"

#include <QStyleOption>
#include <QPainter>

#include "channelcard.h"
#include "typestringdialog.h"
#include "clientsingleton.h"
#include "infodialog.h"
#include "utility.h"
#include "createbroadchatdialog.h"

ChannelList::ChannelList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChannelList)
{
    ui->setupUi(this);

    ui->scrollArea->setWidget(new QWidget());
    ui->scrollArea->widget()->setLayout(new QVBoxLayout());
    ui->scrollArea->widget()->layout()->setAlignment(Qt::AlignTop);

    setupConnection();
}

ChannelList::~ChannelList()
{
    delete ui;
}

void ChannelList::addChannel(const ChannelInfo &channel_info) {
    ChannelCard *channel_card = new ChannelCard(this, channel_info);
    connect(channel_card, &ChannelCard::channelCardClicked, this, &ChannelList::slotHandleChannelCardClicked);

    ui->scrollArea->widget()->layout()->addWidget(channel_card);
}

void ChannelList::removeChannels() {
    ui->searchEdit->setText("");
    QtUtility::clean_layout(ui->scrollArea->widget()->layout());
}

void ChannelList::slotClear() {
    removeChannels();
}

void ChannelList::slotsHandleReturnPress() {
    if (ui->searchEdit->text().isEmpty()) {
        ClientSingleton::get_client()->async_get_channels();
    } else {
        ClientSingleton::get_client()->async_find_channel(ui->searchEdit->text().toStdString());
    }
}

void ChannelList::slotFindChannelResult(int result) {
    removeChannels();
    if (result == FIND_CHANNEL_FAILED_ANSWER) {
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Канал не найден.");
        dialog_ptr->exec();
    } else {
        ChannelInfo channel_info = ClientSingleton::get_client()->get_found_channel();
        addChannel(channel_info);
    }
}

void ChannelList::slotGetChannelsResult(int result, std::vector<ChannelInfo> channels) {
    if (result == GET_CHANNELS_SUCCESS_ANSWER) {
        removeChannels();

        for (int i = 0; i < channels.size(); ++i) {
            addChannel(channels[i]);
        }
    } else {
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Ошибка получения каналов.");
        dialog_ptr->exec();
    }
}

void ChannelList::slotHandleChannelCardClicked(const ChannelInfo &channel_info) {
    emit channelCardClicked(channel_info);
}

void ChannelList::slotCreateChannel() {
    std::unique_ptr<CreateBroadChatDialog> dialog_ptr = std::make_unique<CreateBroadChatDialog>(nullptr, "Введите название канала: ");
    if (dialog_ptr->exec() == QDialog::Accepted) {
        ClientSingleton::get_client()->async_create_channel(dialog_ptr->getInfo());
    }
}

void ChannelList::slotCreateChannelResult(int result) {
    std::string text;
    if (result == CREATE_CHANNEL_SUCCESS_ANSWER) {
        text = "Канал успешно создан.";
        ClientSingleton::get_client()->async_get_channels();
    } else {
        text = "Ошибка создания канала. Попытайтесь позже.";
    }

    std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, text);
    dialog_ptr->exec();
}

void ChannelList::paintEvent(QPaintEvent *) {
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ChannelList::setupConnection() {
    connect(ui->searchEdit,                &QLineEdit::returnPressed,      this, &ChannelList::slotsHandleReturnPress);
    connect(ui->createChannelButton,       &QPushButton::clicked,          this, &ChannelList::slotCreateChannel);
    connect(ClientSingleton::get_client(), &Client::create_channel_result, this, &ChannelList::slotCreateChannelResult);
    connect(ClientSingleton::get_client(), &Client::find_channel_result,   this, &ChannelList::slotFindChannelResult);
    connect(ClientSingleton::get_client(), &Client::get_channels_result,   this, &ChannelList::slotGetChannelsResult);
}
