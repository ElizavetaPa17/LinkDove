#ifndef CHATCARD_H
#define CHATCARD_H

#include <QWidget>

#include "chatinfo.h"

namespace Ui {
class ChatCard;
}

class ChatCard : public QWidget
{
    Q_OBJECT

public:
    explicit ChatCard(QWidget *parent, const ChatInfo &chat_info);
    ~ChatCard();

signals:
    /**
     * <p> Сигнал, который генерируется при нажатии на виджет результата поиска чата. </p.
     * @brief chatCardClicked
     * @param chat_info - Структура с информацией о чате, который содержится в данном виджете.
     */
    void chatCardClicked(const ChatInfo &chat_info);

protected:
    // Переопределяем событие нажатия мыши, чтобы
    // открывать чат с пользователем
    void mousePressEvent(QMouseEvent *event);

    // Переопределяем метод отрисовки, т.к. иначе с виджетом
    // не работают css-таблицы (проблема самого  Qt)
    void paintEvent(QPaintEvent *);

private:
    Ui::ChatCard *ui;
    ChatInfo chat_info_;
};

#endif // CHATCARD_H
