#ifndef CHANNELCARD_H
#define CHANNELCARD_H

#include <QWidget>
#include <QMouseEvent>

#include "channelinfo.h"

namespace Ui {
class ChannelCard;
}

class ChannelCard : public QWidget
{
    Q_OBJECT

public:
    explicit ChannelCard(QWidget *parent, const ChannelInfo &channel_info);
    ~ChannelCard();

signals:
    /**
     * <p> Сигнал, который генерируется при нажатии на виджет результата поиска канала. </p.
     * @brief channelCardClicked
     * @param channel_info -  Структура с информацией о канале, который содержится в данном виджете.
     */
    void channelCardClicked(const ChannelInfo &channel_info);

protected:
    // Переопределяем событие нажатия мыши, чтобы
    // открывать чат с пользователем
    void mousePressEvent(QMouseEvent *event);

    // Переопределяем метод отрисовки, т.к. иначе с виджетом
    // не работают css-таблицы (проблема самого  Qt)
    void paintEvent(QPaintEvent *);

private:
    Ui::ChannelCard *ui;
    ChannelInfo channel_info_;
};

#endif // CHANNELCARD_H
