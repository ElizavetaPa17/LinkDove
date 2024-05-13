#ifndef GROUPWIDGET_H
#define GROUPWIDGET_H

#include <QWidget>

#include "chatinfo.h"
#include "imessage.h"
#include "audiomanager.h"

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
    void slotHandleGetMessages(int result, std::vector<std::shared_ptr<IMessage>> messages);

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

    /** <p> Слот, начинающий запись звука с микрофона устройства для отправки сообщения </p>
     * @brief slotRecordAudio
     */
    void slotRecordAudio();

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
     * <p> Слот, который обрабатывает результат получения списка участников чата (группы). </p>
     * @brief slotGetParticipantListResult
     * @param result - Результат получения списка участников чата (группы).
     * @param participants - Список участников чата (группы).
     */
    void slotGetParticipantListResult(int result, std::vector<std::string> participants);

    /**
     * <p> Слот, который обрабатывает собатие удаление сообщения. </p>
     * @brief slotDeleteMessageResult
     * @param result
     */
    void slotDeleteMessageResult(int result);

private:
    Ui::GroupWidget *ui;
    ChatInfo chat_info_;
    AudioManager audio_manager_;

    int                send_msg_type_;  // Тип крайнего отправленного сообщения
    std::string        image_path_;

    QString audio_file_;
    QString audio_dir_;
    bool is_recording_ = false;

    /**
     * <p> Настраивает соединения c виджетом авторизации (логин-формы). </p>
     * @brief setupConnection
     */
    void setupConnection();
};

#endif // GROUPWIDGET_H
