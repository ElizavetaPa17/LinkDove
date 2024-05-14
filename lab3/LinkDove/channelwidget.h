#ifndef CHANNELWIDGET_H
#define CHANNELWIDGET_H

#include <QWidget>

#include "channelinfo.h"
#include "imessage.h"

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

signals:
    /**
     * <p> Генерирует сигнал при открытии канала. </p>
     * @brief openChannelWidget
     */
    void openChannelWidget();

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
    void slotHandleGetMessages(int result, std::vector<std::shared_ptr<IMessage> > messages);

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

    /** <p> Слот, начинающий запись звука с микрофона устройства для отправки сообщения </p>
     * @brief slotRecordAudio
     */
    void slotRecordAudio();

    /**
     * <p> Слот, который вызывается при нажатии кнопки удаления канала. </p>
     * @brief slotDeleteChannel
     */
    void slotDeleteChannel();

    /**
     * <p> Обрабатывает результат удаления канала. </p>
     * @brief slotHandleDeleteResult
     * @param result - Результат удаления канала.
     */
    void slotHandleDeleteResult(int result);

    /**
     * <p> Слот, который вызывается при нажатии кнопки выхода из канала. </p>
     * @brief slotQuitChannel
     */
    void slotQuitChannel();

    /**
     * <p> Слот, который обрабатывает результат выхода из канала. </p>
     * @brief slotQuitChannelResult
     * @param result - Результат выхода из канала.
     */
    void slotQuitChannelResult(int result);

    /**
     * <p> Слот, который обрабатывает нажатие на кнопку удаление пользователя. </p>
     * @brief slotRemoveUser
     */
    void slotRemoveUser();

    /**
     * <p> Слот, который обрабатывает результат удаление из канала пользователя владельцем канала. </p>
     * @brief slotRemoveUserResult
     * @param result - Результат удаления.
     */
    void slotRemoveUserResult(int result);

    /**
     * <p> Слот, который обрабатывает результат получения списка участников канала. </p>
     * @brief slotGetParticipantListResult
     * @param result - Результат получения списка участников канала.
     * @param participants - Список участников канала.
     */
    void slotGetParticipantListResult(int result, std::vector<std::string> participants);

    /**
     * <p> Слот, который обрабатывает собатие удаление сообщения. </p>
     * @brief slotDeleteMessageResult
     * @param result
     */
    void slotDeleteMessageResult(int result);

private:
    Ui::ChannelWidget *ui;
    ChannelInfo channel_info_;

    int         send_msg_type_;  // Тип крайнего отправленного сообщения
    std::string image_path_;

    QString audio_file_;
    QString audio_dir_;
    bool is_recording_ = false;

    /**
     * <p> Настраивает соединения c виджетом авторизации (логин-формы). </p>
     * @brief setupConnection
     */
    void setupConnection();
};

#endif // CHANNELWIDGET_H
