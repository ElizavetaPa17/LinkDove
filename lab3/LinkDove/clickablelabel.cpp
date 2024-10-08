#include "clickablelabel.h"

ClickableLabel::ClickableLabel(QWidget *parent, Qt::WindowFlags flags)
    : QLabel(parent, flags)
{

}

ClickableLabel::ClickableLabel(const QString& text, QWidget *parent, Qt::WindowFlags flags)
    : QLabel(parent, flags)
{

}

void ClickableLabel::mousePressEvent(QMouseEvent *event) {
    emit clicked(this);

    QLabel::mousePressEvent(event);
}
