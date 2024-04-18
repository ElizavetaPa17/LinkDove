#ifndef MESSAGECARD_H
#define MESSAGECARD_H

#include <QWidget>

namespace Ui {
class MessageCard;
}

class MessageCard : public QWidget
{
    Q_OBJECT

public:
    explicit MessageCard(QWidget *parent = nullptr, QString text="");
    ~MessageCard();

protected:
    // Переопределяем метод отрисовки, т.к. иначе с виджетом
    // не работают css-таблицы (проблема самого  Qt)
    void paintEvent(QPaintEvent *);

private:
    Ui::MessageCard *ui;
};

#endif // MESSAGECARD_H
