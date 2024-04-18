#ifndef USERCARD_H
#define USERCARD_H

#include <QWidget>
#include <QMouseEvent>

#include "StatusInfo.h"

namespace Ui {
class UserCard;
}

class UserCard : public QWidget
{
    Q_OBJECT

public:
    UserCard(QWidget *parent, const StatusInfo &status_info);
    ~UserCard();

signals:
    /**
     * <p> Сигнал, который генерируется при нажатии на виджет результата поиска пользователя. </p.
     * @brief userCardClicked
     * @param status_info
     */
    void userCardClicked(const StatusInfo &status_info);

protected:
    // Переопределяем событие нажатия мыши, чтобы
    // открывать чат с пользователем
    void mousePressEvent(QMouseEvent *event);

    // Переопределяем метод отрисовки, т.к. иначе с виджетом
    // не работают css-таблицы (проблема самого  Qt)
    void paintEvent(QPaintEvent *);

private:
    Ui::UserCard *ui;
    StatusInfo status_info_;
};

#endif // USERCARD_H
