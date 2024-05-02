#include "messagecard.h"
#include "ui_messagecard.h"

#include <QStyleOption>
#include <QPainter>

#include <iostream>

#include "imagedisplayingdialog.h"

MessageCard::MessageCard(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MessageCard)
{
    ui->setupUi(this);
}

MessageCard::MessageCard(QWidget *parent, const QString &text)
    : MessageCard(parent)
{
    ui->label->setText(text);

    image_displaying_ = false;
}

MessageCard::MessageCard(QWidget *parent, const QPixmap &pixmap, const QString &pixmap_path)
    : MessageCard(parent)
{
    ui->label->setPixmap(pixmap);

    image_displaying_ = true;
    pixmap_path_ = pixmap_path;
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
    if (image_displaying_) {
        QPixmap pixmap;
        pixmap.load(pixmap_path_);

        std::unique_ptr<ImageDisplayingDialog> dialog_ptr = std::make_unique<ImageDisplayingDialog>(nullptr, pixmap);
        dialog_ptr->exec();
    }

    QWidget::mousePressEvent(event);
}
