#include "welcomewidget.h"

#include <QLayout>
#include <QPropertyAnimation>
#include <QPainter>
#include <QStyleOption>
#include <QPaintEvent>

#include <iostream>

#include "constants.h"

WelcomeWidget::WelcomeWidget(QWidget *parent, Qt::WindowFlags flags)
    : QWidget(parent, flags)
{
    QPixmap labelPixmap;
    labelPixmap.fill(Qt::transparent);
    labelPixmap.load(":/resources/welcome_label.png");
    labelPixmap = labelPixmap.scaled(labelPixmap.size().width() >> 1, labelPixmap.size().height() >> 1, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    welcomeLabel_ = new TransparentLabel(this);
    welcomeLabel_->setAttribute(Qt::WA_TranslucentBackground);
    welcomeLabel_->setPixmap(labelPixmap);

    QGridLayout *vboxLayout = new QGridLayout(this);
    vboxLayout->addWidget(welcomeLabel_, 0, 0, 1, 1, Qt::AlignJustify);
    setLayout(vboxLayout);

    QPropertyAnimation *animation = new QPropertyAnimation(welcomeLabel_, "p_opacity_", welcomeLabel_);

    connect(animation, &QPropertyAnimation::finished, this, &WelcomeWidget::slotEnableMousePress);
    animation->setDuration(25);
    animation->setStartValue(0);
    animation->setEndValue(1);
    animation->start();
}

void WelcomeWidget::mousePressEvent(QMouseEvent *event) {
    if (!is_press_blocked_) {
        QWidget::mousePressEvent(event);
        emit passWelcomePage(this, LOGIN_PAGE);
    }
}

void WelcomeWidget::slotEnableMousePress() {
    is_press_blocked_ = false;
}
