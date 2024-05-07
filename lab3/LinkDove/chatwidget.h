#ifndef CHATWIDGET_H
#define CHATWIDGET_H

#include <QWidget>
#include "StatusInfo.h"
#include "imessage.h"

namespace Ui {
class ChatWidget;
}

/**
 * @brief The ChatWidget class
 * Класс, отображающий чат между двумя пользователями.
 */
class ChatWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChatWidget(QWidget *parent = nullptr);
    ~ChatWidget();

public slots:
    /**
     * <p> Слот, открывающий чат с заданным пользователем. </p>
     * @brief slotOpenChatWith
     * @param status_info
     */
    void slotOpenChatWith(const StatusInfo &status_info);

    /**
     * <p> Полностью очищает виджет от сообщений. </p>
     * @brief slotClear
     */
    void slotClear();

signals:
    /**
     * <p> Генерирует сигнал при открытии чата. </p>
     * @brief openChatWidget
     */
    void openChatWidget();

private slots:
    /**
     * <p> Слот, обрабатывающий отправку сообщения. </p>
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
    void slotHandleGetMessages(int result, std::vector<std::shared_ptr<IMessage>> messages);

    /**
     * <p> Слот, который открывает диалоговое окно ддя выбора фотографии с целью его отправления в чат. </p>
     * @brief slotChooseImage
     */
    void slotChooseImage();

    /**
     * <p> Отображает профиль собеседника. </p>
     * @brief slotDisplayInterlocutorProfile
     */
    void slotDisplayInterlocutorProfile();

    /**
     * <p> Слот, который вызывается при нажатии кнопки удаления чата. </p>
     * @brief slotDeleteChat
     */
    void slotDeleteChat();

    /**
     * <p> Обрабатывает результат удаления чата. </p>
     * @brief slotHandleDeleteResult
     * @param result - Результат удаления чата.
     */
    void slotHandleDeleteResult(int result);

private:
    Ui::ChatWidget    *ui;
    StatusInfo interlocutor_;

    int                send_msg_type_;  // Тип крайнего отправленного сообщения
    std::string        image_path_;

    /**
     * <p> Настраивает соединения c виджетом авторизации (логин-формы). </p>
     * @brief setupConnection
     */
    void setupConnection();
};

#endif // CHATWIDGET_H
