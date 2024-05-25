#ifndef REQUESTLISTDIALOG_H
#define REQUESTLISTDIALOG_H

#include <QDialog>

class QScrollArea;

/**
 * @brief The RequestListDialog class
 * Виджет отображения запросов.
 */
class RequestListDialog : public QDialog
{
    Q_OBJECT
public:
    RequestListDialog(QWidget *parent = nullptr);
    ~RequestListDialog();

    /**
     * <p> Добавляет никнейм в отображаемый список. </p>
     * @brief addRequestUsername
     * @param username - Добавляемый никнейм.
     */
    void addRequestUsername(const std::string &username);

    /**
     * <p> Удаляет все запросы из отображаемого списка. </p>
     * @brief removeAllRequests
     */
    void removeAllRequests();

    /**
     * <p> Устанавливает информацию о чате. </p>
     * @brief setBroadChatId
     */
    void setBroadChatInfo(unsigned long long broad_chat_id, bool is_channel);

private slots:
    /**
     * <p> Слот, который обрабатывает попытку удалить запрос из виджета. </p>
     * @brief slotRemoveRequest
     * @param username - Никнейм пользователя удаляемого запроса.
     */
    void slotRemoveRequest(std::string username);

    /**
     * <p> Слот, который обрабатывает попытку добавить пользователя в приватный канал. </p>
     * @brief slotAddParticipant
     * @param username - Никнейм добавляемого пользователя.
     */
    void slotAddParticipant(std::string username);

    /**
     * <p> Слот, который обрабатывает результат запроса на удаление запроса на вступление в приватный канал. </p>
     * @brief slotRemoveRequestResult
     * @param result - Результат запроса.
     */
    void slotRemoveRequestResult(int result);

    /**
     * <p> Слот, который обрабатывает результат запроса на добавление пользователя в приватный канал. </p>
     * @brief slotRemoveRequestResult
     * @param result - Результат запроса.
     */
    void slotAddParticipantResult(int result);

private:
    QScrollArea *scroll_area_;
    unsigned long long broad_chat_id_;
    bool is_channel_;
    std::string removed_request_username_;

    /**
     * <p> Удаляет запрос из отображаемого списка. </p>
     * @brief removeComplaint
     * @param complaint_id - Идентификатор удаляемого запроса.
     */
    void removeRequest(const std::string &username);

    /**
     * <p> Настраивает соединения внутри виджета. </P>
     * @brief setupConnection
     */
    void setupConnections();
};

#endif // REQUESTLISTDIALOG_H
