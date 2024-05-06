#ifndef GROUPWIDGET_H
#define GROUPWIDGET_H

#include <QWidget>

#include "chatinfo.h"

namespace Ui {
class GroupWidget;
}

/**
 * @brief The GroupWidget class
 * Класс, отображающий группу.
 */
class GroupWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GroupWidget(QWidget *parent = nullptr);
    ~GroupWidget();

public slots:
    /**
     * <p> Слот, открывающий заданную группу. </p>
     * @brief slotOpenGroup
     * @param channel_info - Структура, содержащая информацию о группе.
     */
    void slotOpenGroup(const ChatInfo &chat_info);

    /**
     * <p> Слот, обрабатывающий результат получения информации о том, является ли пользователь участником группы. </p>
     * @brief slotHandleIsGroupParticipantResult
     * @param result - Параметр, содержащий результат получения информации.
     * @param is_participant - Флаг, содержащий информацию в случае ее успешного получения.
     */
    void slotHandleIsGroupParticipantResult(int result, bool is_participant);

    /**
     * <p> Полностью очищает виджет от сообщений. </p>
     * @brief slotClear
     */
    void slotClear();

signals:
    /**
     * <p> Генерирует сигнал при открытии группы. </p>
     * @brief openGroupWidget
     */
    void openGroupWidget();

private slots:
    /**
     * <p> Слот, обрабатывающий отправку сообщения от владельца канала. </p>
     * @brief slotSendMessage
     */
    void slotSendMessage();

    /**
     * <p> Слот, обрабатывающий результат отправки сообщения. </p>
     * @brief slotHandleSendMessage
     * @param result - Параметр, содержащий результат отправки сообщения.
     */
    void slotHandleSendMessage(int result);

    /**
     * <p> Слот, обрабатывающий результат получения списка сообщеий. </p>
     * @brief slotHandleGetMessage
     * @param result - Параметр, содержащий результат получения сообщений.
     */
    void slotHandleGetMessages(int result);

    /**
     * <p> Слот, обрабатывающий результат добавление текущего пользователя в группу. </p>
     * @brief slotHandleAddParticipantGroupResult
     * @param result - Параметр, содержащий результат добавление пользователя.
     */
    void slotHandleAddParticipantGroupResult(int result);

    /**
     * <p> Слот, который открывает диалоговое окно ддя выбора фотографии с целью его отправления в чат. </p>
     * @brief slotChooseImage
     */
    void slotChooseImage();

    /**
     * <p> Слот, который вызывается при нажатии кнопки удаления чата (группы). </p>
     * @brief slotDeleteGroup
     */
    void slotDeleteGroup();

    /**
     * <p> Слот, который обрабатывает результат удаления группы. </p>
     * @brief slotHandleDeleteResult
     * @param result - Результат удаления группы.
     */
    void slotHandleDeleteResult(int result);

    /**
     * <p> Слот, который вызывается при нажатии кнопки выхода из чата (группы). </p>
     * @brief slotQuitGroup
     */
    void slotQuitGroup();

    /**
     * <p> Слот, который обрабатывает результат выхода из группы. </p>
     * @brief slotQuitGroupResult
     * @param result
     */
    void slotQuitGroupResult(int result);

private:
    Ui::GroupWidget *ui;
    ChatInfo chat_info_;

    int                send_msg_type_;  // Тип крайнего отправленного сообщения
    std::string        image_path_;

    /**
     * <p> Настраивает соединения c виджетом авторизации (логин-формы). </p>
     * @brief setupConnection
     */
    void setupConnection();
};

#endif // GROUPWIDGET_H
