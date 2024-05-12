#include "messagecard.h"
#include "ui_messagecard.h"

#include <QStyleOption>
#include <QPainter>

#include <iostream>

#include "constants.h"
#include "imagedisplayingdialog.h"
#include "audiomanagersingleton.h"

MessageCard::MessageCard(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MessageCard)
{
    ui->setupUi(this);
    setupConnection();
}

MessageCard::MessageCard(QWidget *parent, int type, const QString &text)
    : MessageCard(parent)
{
    switch (type) {
        case TEXT_MSG_TYPE: {
            ui->label->setText(text);
            ui->groupBox->hide();
            break;
        }
        case AUDIO_MSG_TYPE: {
            string_ = text;
            ui->label->hide();
        }
    }

    ui->ownerLabel->hide();
    ui->line->hide();
}

MessageCard::MessageCard(QWidget *parent, const QPixmap &pixmap, const QString &pixmap_path)
    : MessageCard(parent)
{
    type_ = IMAGE_MSG_TYPE;
    string_ = pixmap_path;

    ui->label->setPixmap(pixmap);
    ui->groupBox->hide();
    ui->ownerLabel->hide();
    ui->line->hide();

    cursor().setShape(Qt::PointingHandCursor);
}

MessageCard::MessageCard(QWidget *parent, int type, const QString &text, const QString &owner_name)
    : MessageCard(parent, type, text)
{    
    ui->ownerLabel->setText(owner_name);
    ui->ownerLabel->show();
    ui->line->show();
}

MessageCard::MessageCard(QWidget *parent, const QPixmap &pixmap, const QString &pixmap_path, const QString &owner_name)
    : MessageCard(parent, pixmap, pixmap_path)
{
    ui->ownerLabel->setText(owner_name);
    ui->ownerLabel->show();
    ui->line->show();
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
    if (type_ == IMAGE_MSG_TYPE) {
        QPixmap pixmap;
        pixmap.load(string_);

        std::unique_ptr<ImageDisplayingDialog> dialog_ptr = std::make_unique<ImageDisplayingDialog>(nullptr, pixmap);
        dialog_ptr->exec();
    }

    QWidget::mousePressEvent(event);
}

void MessageCard::slotPlayAudio() {
    ui->audioButton->setIcon(QIcon(":/recources/../resources/audio_play_icon.png"));
    AudioManagerSingleton::get_manager()->play(string_);
}

void MessageCard::slotStopAudio() {
    ui->audioButton->setIcon(QIcon(":/recources/../resources/audio_wait_icon.png"));
}

void MessageCard::setupConnection() {
    connect(ui->audioButton, &QPushButton::clicked, this, &MessageCard::slotPlayAudio);
    connect(AudioManagerSingleton::get_manager(), &AudioManager::playing_stopped, this, &MessageCard::slotStopAudio);
}
