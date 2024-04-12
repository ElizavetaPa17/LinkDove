#ifndef USERSLIST_H
#define USERSLIST_H

#include <QWidget>

namespace Ui {
class UsersList;
}

class UsersList : public QWidget
{
    Q_OBJECT

public:
    explicit UsersList(QWidget *parent = nullptr);
    ~UsersList();

protected:
    // Переопределяем метод отрисовки, т.к. иначе с виджетом
    // не работают css-таблицы (проблема самого  Qt)
    void paintEvent(QPaintEvent *);

private slots:
    /**
     * <p> Слот, обратабывающий событие нажатия клавиши Enter. </p>
     * @brief slotsHandleReturnPress
     */
    void slotsHandleReturnPress();

    /**
     * <p> Слот, принимающий от клиента результата поиска данных о пользователе. </p>
     * @brief slotDisplayFindUserResult
     * @param result - Результат поиска данных о пользователе.
     */
    void slotFindUserResult(int result);

private:
    Ui::UsersList *ui;

    /**
     * <p> Настраивает соединения с классом. </p>
     * @brief setupConnections
     */
    void setupConnection();
};

#endif // USERSLIST_H
