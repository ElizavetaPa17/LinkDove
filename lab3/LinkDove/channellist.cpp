#include "channellist.h"
#include "ui_channellist.h"

#include <QStyleOption>
#include <QPainter>

#include "channelcard.h"
#include "createchanneldialog.h"
#include "clientsingleton.h"
#include "utility.h"

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
        // search for all the channels
    } else {
       /* if (ui->searchEdit->text().toStdString() != ClientSingleton::get_client()->get_status_info().username_) {
            ClientSingleton::get_client()->async_find_user(ui->searchEdit->text().toStdString());
        } else {
            std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Имя пользователя не может совпадать с Вашим.");
            dialog_ptr->exec();
        }*/
    }
}

void ChannelList::slotFindChannelResult(int result) {
    removeChannels();
    /*    if (result == FIND_USER_FAILED_ANWSER) {
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Пользователь не найден.");
        dialog_ptr->exec();
    } else {
        StatusInfo status_info = ClientSingleton::get_client()->get_found_user();
        addUser(status_info);
    }*/
}

void ChannelList::slotHandleChannelCardClicked(const ChannelInfo &channel_info) {

}

void ChannelList::slotCreateChannel() {
    std::unique_ptr<CreateChannelDialog> dialog_ptr = std::make_unique<CreateChannelDialog>();
    if (dialog_ptr->exec() == QDialog::Accepted) {
        ClientSingleton::get_client()->async_create_channel(dialog_ptr->getChannelName());
    }
}

void ChannelList::paintEvent(QPaintEvent *) {
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ChannelList::setupConnection() {
    connect(ui->searchEdit, &QLineEdit::returnPressed, this, &ChannelList::slotsHandleReturnPress);
    connect(ui->createChannelButton, &QPushButton::clicked, this, &ChannelList::slotCreateChannel);
}
