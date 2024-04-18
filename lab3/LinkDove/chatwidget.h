#ifndef CHATWIDGET_H
#define CHATWIDGET_H

#include <QWidget>
#include "StatusInfo.h"

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

private:
    Ui::ChatWidget *ui;
    unsigned long long receiver_id_;

    /**
     * <p> Настраивает соединения c виджетом авторизации (логин-формы). </p>
     * @brief setupConnection
     */
    void setupConnection();
};

#endif // CHATWIDGET_H
