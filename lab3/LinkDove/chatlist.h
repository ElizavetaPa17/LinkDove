#ifndef CHATLIST_H
#define CHATLIST_H

#include <QWidget>

#include "chatinfo.h"

namespace Ui {
class ChatList;
}

/**
 * @brief The ChatList class
 * Отображает список чатов и позволяет создать новый чат.
 */
class ChatList : public QWidget
{
    Q_OBJECT

public:
    explicit ChatList(QWidget *parent = nullptr);
    ~ChatList();

    /**
     * <p> Добавляет информацию о чате в список. </p>
     * @brief addChat
     * @param chat_info - Структура, содержащая информацию о чате.
     */
    void addChat(const ChatInfo &chat_info);

    /**
     * <p> Удаляет все чаты из списка. </p>
     * @brief removeChats
     */
    void removeChats();

signals:
    /**
     * <p> Генерирует сигнал при нажатии на виджет результата поиска чата. </p>
     * @brief chatCardClicked
     * @param chat_info - Структура, содержащая информацию о чате.
     */
    void chatCardClicked(const ChatInfo &chat_info);

public slots:
    /**
     * <p> Полностью очищает виджет от объектов ChatCard. </p>
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
     * <p> Слот, принимающий от клиента результат поиска данных о чате. </p>
     * @brief slotDisplayFindChatResult
     * @param result - Результат поиска данных о канале.
     */
    void slotFindChatResult(int result);

    /**
     * <p> Слот, принимающий от клиента результат получения информации о чатах, в которых состоит текущий пользователь. </p>
     * @brief slotGetChatsResult
     * @param result - Результат получения информации о таких чатах.
     */
    void slotGetChatsResult(int result);

    /**
     * <p> Слот, обрабатывающий событие нажатие кнопки создания чата. </p>
     * @brief slotCreateChat
     */
    void slotCreateChat();

    /**
     * <p> Слот, принимающий от клиента результат создания чата. </p>
     * @brief slotCreateChatResult
     * @param result - Результат создания канала.
     */
    void slotCreateChatResult(int result);

    /**
     * <p> Слот, принимающий события нажатия на виджет результата поиска чата. </p>
     * @brief slotHandleChatCardClicked
     * @param chat_info - Структура, содержащая информацию о чате.
     */
    void slotHandleChatCardClicked(const ChatInfo &chat_info);

protected:
    // Переопределяем метод отрисовки, т.к. иначе с виджетом
    // не работают css-таблицы (проблема самого  Qt)
    void paintEvent(QPaintEvent *);

private:
    Ui::ChatList *ui;

    /**
     * <p> Настраивает соединения внутри виджета. </p>
     * @brief setupConnection
     */
    void setupConnection();
};

#endif // CHATLIST_H
