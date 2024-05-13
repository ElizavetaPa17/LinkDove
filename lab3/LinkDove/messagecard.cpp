#include "messagecard.h"
#include "ui_messagecard.h"

#include <QStyleOption>
#include <QPainter>

#include <iostream>

#include "constants.h"
#include "imagedisplayingdialog.h"
#include "audiomanagersingleton.h"
#include "groupmessage.h"
#include "agreedialog.h"
#include "clientsingleton.h"

MessageCard::MessageCard(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MessageCard)
{
    ui->setupUi(this);
    setupConnection();
}

MessageCard::MessageCard(QWidget *parent, std::shared_ptr<IMessage> msg, bool can_delete)
    : MessageCard(parent)
{
    msg_ = msg;

    switch (msg->get_msg_content()->get_msg_content_type()) {
        case TEXT_MSG_TYPE: {
            ui->label->setText(msg->get_msg_content()->get_raw_data());
            ui->groupBox->hide();
            break;
        }
        case IMAGE_MSG_TYPE: {
            QPixmap pix;
            pix.load(msg_->get_msg_content()->get_raw_data());
            pix = pix.scaledToWidth(450);

            ui->label->setPixmap(pix);
            ui->groupBox->hide();

            cursor().setShape(Qt::PointingHandCursor);
            break;
        }
        case AUDIO_MSG_TYPE: {
            ui->label->hide();
            break;
        }
    }

    switch (msg->get_msg_type()) {
        case CHANNEL_MSG_TYPE:
        case INDIVIDUAL_MSG_TYPE: {
            ui->ownerLabel->hide();
            ui->line->hide();
            break;
        }
        case GROUP_MSG_TYPE: {
             ui->ownerLabel->setText(std::dynamic_pointer_cast<GroupMessage>(msg)->get_owner_name().c_str());
             ui->ownerLabel->show();
             ui->line->show();
             break;
        }
    }

    if (can_delete) {
        std::cerr << can_delete;
        ui->deleteButton->show();
    } else {
        ui->deleteButton->hide();
    }
}

MessageCard::~MessageCard()
{
    delete ui;
}

void MessageCard::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void MessageCard::mousePressEvent(QMouseEvent *event) {
    if (msg_->get_msg_content()->get_msg_content_type() == IMAGE_MSG_TYPE) {
        QPixmap pixmap;
        pixmap.load(msg_->get_msg_content()->get_raw_data());

        std::unique_ptr<ImageDisplayingDialog> dialog_ptr = std::make_unique<ImageDisplayingDialog>(nullptr, pixmap);
        dialog_ptr->exec();
    }

    QWidget::mousePressEvent(event);
}

void MessageCard::slotPlayAudio() {
    ui->audioButton->setIcon(QIcon(":/recources/../resources/audio_play_icon.png"));
    AudioManagerSingleton::get_manager()->play(msg_->get_msg_content()->get_raw_data());
}

void MessageCard::slotStopAudio() {
    ui->audioButton->setIcon(QIcon(":/recources/../resources/audio_wait_icon.png"));
}

void MessageCard::slotDeleteMsg() {
    std::unique_ptr<AgreeDialog> dialog_ptr = std::make_unique<AgreeDialog>(nullptr, "Вы точно хотите удалить сообщение?");
    if (dialog_ptr->exec() == QDialog::Accepted) {
        ClientSingleton::get_client()->async_delete_msg(*msg_);
    }
}

void MessageCard::setupConnection() {
    connect(ui->audioButton,                      &QPushButton::clicked,          this, &MessageCard::slotPlayAudio);
    connect(ui->deleteButton,                     &QPushButton::clicked,          this, &MessageCard::slotDeleteMsg);
    connect(AudioManagerSingleton::get_manager(), &AudioManager::playing_stopped, this, &MessageCard::slotStopAudio);
}
