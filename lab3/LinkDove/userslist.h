 #ifndef USERSLIST_H
#define USERSLIST_H

#include <QWidget>

#include "StatusInfo.h"
#include "list"

namespace Ui {
class UsersList;
}

/**
 * @brief The UsersList class
 * Виджет, отображающий список пользователей.
 */
class UsersList : public QWidget
{
    Q_OBJECT

public:
    explicit UsersList(QWidget *parent = nullptr);
    ~UsersList();

    /**
     * <p> Добавляет информацию о пользователе в список. </p>
     * @brief addUser
     * @param status_info - Структура, содержащая информацию о пользователе.
     */
    void addUser(const StatusInfo &status_info);

    /**
     * <p> Удаляет всех пользователей из списка. </p>
     * @brief removeUsers
     */
    void removeUsers();

signals:
    /**
     * <p> Сигнал, который генерируется при нажатии на виджет результата поиска пользователя. </p.
     * @brief userCardClicked
     * @param status_info - Структура, содержащая информацию о пользователе.
     */
    void userCardClicked(const StatusInfo &status_info);

public slots:
    /**
     * <p> Полностью очищает виджет от объектов UserCard. </p>
     * @brief slotClear
     */
    void slotClear();

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

    /**
     * <p> Слот, принимающий от клиента результат получения информации о собеседниках текущего пользователя. </p>
     * @brief slotGetInterlocutorsResult
     * @param result - Результат получения информации.
     */
    void slotGetInterlocutorsResult(int result, std::vector<StatusInfo> interlocutors);

    /**
     * <p> Слот, принимающий события нажатия на виджет результата поиска пользователя. </p>
     * @brief slotHandleUserCardClicked
     * @param status_info
     */
    void slotHandleUserCardClicked(const StatusInfo &status_info);

    /**
     * <p> Слот, принимающий от клиента результат получения статуса блокировки пользователя. </p>
     * @brief slotHandleIsBannedUser
     * @param result - Результат получения статуса блокировки.
     * @param is_banned - Флаг статуса блокировки.
     */
    void slotHandleIsBannedUser(int result, bool is_banned);

protected:
    // Переопределяем метод отрисовки, т.к. иначе с виджетом
    // не работают css-таблицы (проблема самого  Qt)
    void paintEvent(QPaintEvent *);

private:
    Ui::UsersList *ui;
    StatusInfo status_info_;

    /**
     * <p> Настраивает соединения с классом. </p>
     * @brief setupConnections
     */
    void setupConnection();
};

#endif // USERSLIST_H
