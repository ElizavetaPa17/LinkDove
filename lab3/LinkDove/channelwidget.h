#ifndef CHANNELWIDGET_H
#define CHANNELWIDGET_H

#include <QWidget>
#include "channelinfo.h"

namespace Ui {
class ChannelWidget;
}

/**
 * @brief The ChannelWidget class
 * Класс, отображающий канал.
 */
class ChannelWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChannelWidget(QWidget *parent = nullptr);
    ~ChannelWidget();

public slots:
    /**
     * <p> Слот, открывающий заданный канал. </p>
     * @brief slotOpenChannel
     * @param channel_info - Структура, содержащая информацию о канале.
     */
    void slotOpenChannel(const ChannelInfo &channel_info);

    /**
     * <p> Слот, обрабатывающий результат получения информации о том, является ли пользователь участником канала. </p>
     * @brief slotHandleIsChannelParticipantResult
     * @param result - Параметр, содержащий результат получения информации.
     * @param is_participant - Флаг, содержащий информацию в случае ее успешного получения.
     */
    void slotHandleIsChannelParticipantResult(int result, bool is_participant);

    /**
     * <p> Полностью очищает виджет от сообщений. </p>
     * @brief slotClear
     */
    void slotClear();

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
     * <p> Слот, обрабатывающий результат добавление текущего пользователя в канал. </p>
     * @brief slotHandleAddParticipantChannelResult
     * @param result - Параметр, содержащий результат добавление пользователя.
     */
    void slotHandleAddParticipantChannelResult(int result);

    /**
     * <p> Слот, который открывает диалоговое окно ддя выбора фотографии с целью его отправления в чат. </p>
     * @brief slotChooseImage
     */
    void slotChooseImage();

private:
    Ui::ChannelWidget *ui;
    ChannelInfo channel_info_;

    int                send_msg_type_;  // Тип крайнего отправленного сообщения
    std::string        image_path_;

    /**
     * <p> Настраивает соединения c виджетом авторизации (логин-формы). </p>
     * @brief setupConnection
     */
    void setupConnection();
};

#endif // CHANNELWIDGET_H
