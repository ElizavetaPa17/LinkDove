#include "transparentlabel.h"

TransparentLabel::TransparentLabel(QWidget *parent, Qt::WindowFlags flags)
    : QLabel(parent, flags)
{
    setup();
}

TransparentLabel::TransparentLabel(const QString &text, QWidget *parent, Qt::WindowFlags flags )
    : QLabel(text, parent, flags)
{
    setup();
}

void TransparentLabel::setup() {
    goe_ = new QGraphicsOpacityEffect(this);
    goe_->setOpacity(1.0);
    setGraphicsEffect(goe_);
}

void TransparentLabel::setOpacity(double value) {
    goe_->setOpacity(value);
    emit opacityChanged(goe_->opacity());
}

double TransparentLabel::Opacity() const {
    return p_opacity_;
}
