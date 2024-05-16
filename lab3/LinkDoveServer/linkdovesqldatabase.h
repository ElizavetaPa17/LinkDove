#ifndef LINKDOVESQLDATABASE_H
#define LINKDOVESQLDATABASE_H

#include <QtSql/QSqlDatabase>

#include "UserInfo.h"
#include "logininfo.h"
#include "channelinfo.h"
#include "chatinfo.h"
#include "complaint.h"
#include "imessage.h"
#include "notification.h"

class LinkDoveSQLDataBase
{
public:
    LinkDoveSQLDataBase(const std::string &connection_name);
    ~LinkDoveSQLDataBase();

    /**
     * <p> Добавляет пользователя в БД. </p>
     * @brief register_user
     * @param info - Информация о пользователе, который добавляется в БД.
     * @return - В случае успеха возвращает true, иначе - false.
     */
    bool register_user(const UserInfo& info);

    /**
     * <p> Проверяет, зарегистрирован ли пользователь в БД. </p>
     * @brief login_user
     * @param info - Информация пользователе, который проверяется на наличие в БД.
     * @return - В случае наличия пользователя в БД возвращает true, иначе - false.
     */
    bool login_user(const LoginInfo& info);

    /**
     * <p> Обновляет данные о пользователе. </p>
     * @brief update_user
     * @param status_info - Структура, содержащая обновленную информацию о пользователе.
     * @return - В случае успешного обновления информации в БД возвращает true, иначе - false.
     */
    bool update_user(const StatusInfo& status_info);

    /**
     * <p> Блокирует или разблокировывает пользователя для авторизации. </p>
     * @brief ban_user
     * @param username - Никнейм пользователя, который блокируется/разблокируется.
     * @param is_ban - Если is_ban равно true, то пользователь блокируется, иначе - разблокировывается.
     * @return - В случае успешного обновления статуса блокировки true, иначе - false.
     */
    bool ban_user(const std::string &username, bool is_ban);

    /**
     * <p> Блокирует или разблокировывает пользователя в переписке. </p>
     * @brief ban_ind_user
     * @param from_id - Идентификатор пользователя, который осуществляет блокировку.
     * @param to_id - Идентификатор пользователя, который блокируется/разблокируется.
     * @param is_ban - Если is_ban равно true, то пользователь блокируется, иначе - разблокировывается.
     * @return - В случае успешного обновления статуса блокировки true, иначе - false.
     */
    bool ban_ind_user(unsigned long long from_id, unsigned long long to_id, bool is_ban);

    /**
     * <p> Возвращает статус блокировки пользователя в переписке. </p>
     * @brief is_banned_ind_user
     * @param from_id - Идентификатор пользователя, который заблокировал.
     * @param to_id - Идентификатор пользователя, который был заблокирован.
     * @return - В случае блокировки возвращает true, иначе - false.
     */
    bool is_banned_ind_user(unsigned long long from_id, unsigned long long to_id);

    /**
     * <p> Проверяет, блокирован ли пользователь. </p>
     * @brief get_user_banned_status
     * @param username
     * @return - True, если пользователь блокирован, иначе - false.
     */
    bool get_user_banned_status(const std::string &username);

    /**
     * <p> Добавляет жалобу в БД. </p>
     * @brief add_complaint
     * @param complaint - Информация о жалобе, которая добавляется в БД.
     * @return - В случае успеха возвращает true, иначе - false.
     */
    bool add_complaint(const Complaint& complaint);

    /**
     * <p> Удаляет жалобу из БД. </p>
     * @brief del_complaint
     * @param complaint_id - Идентификатор удаляемой жалобы.
     * @return - В случае успеха возвращает true, иначе - false.
     */
    bool del_complaint(unsigned long long complaint_id);

    /**
     * <p> Запрос на количество жалоб. </p>
     * @brief get_complaints_count
     * @return - Возвращает количество жалоб.
     */
    int get_complaints_count();

    /**
     * <p> Извлекает первые count жалоб. </p>
     * @brief get_complaints
     * @param count - Количество жалоб для извлечения.
     * @return - Вектор, содержащий не более count жалоб.
     */
    std::vector<Complaint> get_complaints(int count);

    /**
     * <p> Добавляет в БД ответ на жалобу пользователя. </p>
     * @brief add_answer
     * @param user_id - Идентификатор пользователя, которому отправляется ответ на жалобу.
     * @param text - Текстовое содержимое ответа.
     * @return - В случае успеха возвращает true, иначе - false.
     */
    bool add_answer(unsigned long long user_id, const std::string &text);

    /**
     * <p> Извлекает первые count уведомлений пользователя. </p>
     * @brief get_notifications
     * @param user_id - Идентификатор пользователя, чьи уведомления извлекаются.
     * @param count - Количество уведомлений для извлечения
     * @return - Вектор, содержащий не более count уведомлений.
     */
    std::vector<Notification> get_notifications(unsigned long long user_id, int count);

    /**
     * <p> Удаляет уведомление из БД. </p>
     * @brief delete_notification
     * @param id - Идентификатор удаляемого уведомления.
     * @return - В случае успеха возвращает true, иначе - false.
     */
    bool delete_notification(unsigned long long id);

    /**
     * <p> Добавляет сообщение между двумя собеседниками в БД. </p>
     * @brief add_ind_msg
     * @param msg - Добавляемое сообщение.
     * @return - В случае успеха возвращает true, иначе - false.
     */
    bool add_ind_message(const IMessage &msg);

    /**
     * <p> Удаляет сообщение между двумя собеседниками. </p>
     * @brief delete_ind_message
     * @param msg - Удаляемое сообщение.
     * @return - В случае успеха возвращает true, иначе - false.
     */
    bool delete_ind_message(const IMessage &msg);

    /**
     * <p> Добавляет сообщение в канал. </p>
     * @brief add_chnnl_message
     * @param msg - Добавляемое сообщение.
     * @return - В случае успеха возвращает true, иначе - false.
     */
    bool add_chnnl_message(const IMessage& msg);

    /**
     * <p> Удаляет сообщение из канала. </p>
     * @brief delete_chnnl_message
     * @param msg - Удаляемое сообщение.
     * @return - В случае успеха возвращает true, иначе - false.
     */
    bool delete_chnnl_message(const IMessage& msg);

    /**
     * <p> Возвращает сообщения между двумя собеседниками. </p>
     * @brief get_ind_messages
     * @param sender_id - Идентификатор отправителя сообщений.
     * @param receiver_id - Идентификатор получателя сообщений.
     * @return
     */
    std::vector<std::shared_ptr<IMessage>> get_ind_messages(unsigned long long sender_id, unsigned long long receiver_id);

    /**
     * <p> Возвращает информацию о собеседниках пользователя. </p>
     * @brief get_interlocutors
     * @param id - Идентификатор пользователя, информация о собеседниках которого извлекается.
     * @return - Вектор из структур, содержащих информацию о собеседниках.
     */
    std::vector<StatusInfo> get_interlocutors(unsigned long long id);

    /**
     * <p> Удаляет переписку между двумя пользователями. </p>
     * @brief delete_ind_chat
     * @param first_id - Идентификатор первого собеседника.
     * @param second_id - Идентификатор второго собеседника.
     * @return - В случае успеха - true, иначе - false.
     */
    bool delete_ind_chat(unsigned long long first_id, unsigned long long second_id);

    /**
     * <p> Добавляет канал в БД. </p>
     * @brief add_channel
     * @param channel_info - Добавляемый канал.
     * @return - - В случае успеха возвращает true, иначе - false.
     */
    bool add_channel(const ChannelInfo &channel_info);

    /**
     * <p> Возвращает информацию о каналах, в которых состоит пользователь. </p>
     * @brief get_channels
     * @param id - Идентификатор пользователя, информация о каналах которого извлекается.
     * @return  - Вектор из структур, содержащих информацию о каналах.
     */
    std::vector<ChannelInfo> get_channels(unsigned long long id);

    /**
     * <p> Возвращает информацию о канале, который соответствует заданному названию. </p>
     * @brief get_channel
     * @param channel_name - Название канала.
     * @return  - Структура, содержащая информацию о канале.
     */
    ChannelInfo get_channel(const std::string &channel_name);

    /**
     * <p> Возвращает информацию о канале, который соответствует заданному идентификатору. </p>
     * @brief get_channel
     * @param channel_id - Идентификатор канала.
     * @return - Структура, содержащая информацию о канале.
     */
    ChannelInfo get_channel(unsigned long long channel_id);

    /**
     * <p> Добавляет пользователя в канал. </p>
     * @brief add_participant_to_channel
     * @param participant_id - Идентификатор добавляемого пользователя.
     * @param channel_id - Идентификатор канала, в который добавляется пользователь.
     * @return - В случае успеха возвращает true, иначе - false.
     */
    bool add_participant_to_channel(unsigned long long participant_id, unsigned long long channel_id);

    /**
     * <p> Проверяет, является ли пользователь участником канала. </p>
     * @brief is_channel_participant
     * @param participant_id - Идентификатор пользователя.
     * @param channel_id - Идентификатор канала.
     * @return - В случае положительного ответа возвращает true, иначе - false.
     */
    bool is_channel_participant(unsigned long long participant_id, unsigned long long channel_id);

    /**
     * <p> Извлекает сообщения канала из БД. </p>
     * @brief get_channel_messages
     * @param channel_id - Идентификатор канала, из которого извлекаются сообщения.
     * @return - Вектор сообщений.
     */
    std::vector<std::shared_ptr<IMessage> > get_channel_messages(unsigned long long channel_id);

    /**
     * <p> Возвращает список никнеймов участников канала. </p>
     * @brief get_channel_participants
     * @param channel_id - Идентификатор канала, из которого извлекаются участники.
     * @return - Список участников канала.
     */
    std::vector<std::string> get_channel_participants(unsigned long long channel_id);

    /**
     * <p> Удаляет канал из БД. </p>
     * @brief delete_channel
     * @param channel_id - Идентификатор удаляемого канала.
     * @return - В случае успеха возвращает true, иначе - false.
     */
    bool delete_channel(unsigned long long channel_id);

    /**
     * <p> Удаляет пользователя из чата канала. </p>
     * @brief quit_channel
     * @param user_id - Идентификатор удаляемого из канала пользователя.
     * @param chat_id - Идентификатор канала, из которого удаляется пользователь.
     * @return - В случае успеха возвращает true, иначе - false.
     */
    bool quit_channel(unsigned long long user_id, unsigned long long channel_id);

    /**
     * <p> Добавляет чат в БД. </p>
     * @brief add_chat
     * @param chat_info - Структура, содержащая информацию о чате.
     * @return  - В случае положительного ответа возвращает true, иначе - false.
     */
    bool add_chat(const ChatInfo &chat_info);

    /**
     * <p> Извлекает информацию о чате. </p>
     * @brief get_chat
     * @param chat_name - Имя чата.
     * @return - Структура, содержащая информацию о чате.
     */
    ChatInfo get_chat(const std::string &chat_name);

    /**
     * <p> Извлекает информацию о чате. </p>
     * @brief get_chat
     * @param id - Идентификатор чата.
     * @return - Структура, содержащая информацию о чате.
     */
    ChatInfo get_chat(unsigned long long id);

    /**
     * <p> Добавляет пользователя в чат. </p>
     * @brief add_participant_to_chat
     * @param participant_id - Идентификатор добавляемого пользователя.
     * @param chat_id - Идентификатор чата.
     * @return - В случае успеха возвращает true, иначе - false.
     */
    bool add_participant_to_chat(unsigned long long participant_id, unsigned long long chat_id);

    /**
     * <p> Проверяет, является ли пользователь участником чата. </p>
     * @brief is_chat_participant
     * @param participant_id - Идентификатор пользователя.
     * @param chat_id - Идентификатор чата.
     * @return - В случае положительного ответа возвращает true, иначе - false.
     */
    bool is_chat_participant(unsigned long long participant_id, unsigned long long chat_id);

    /**
     * <p> Возвращает информацию о чатах, в которых состоит пользователь. </p>
     * @brief get_chats
     * @param id - Идентификатор пользователя, информация о чатах которого извлекается.
     * @return  - Вектор из структур, содержащих информацию о чатах.
     */
    std::vector<ChatInfo> get_chats(unsigned long long id);

    /**
     * <p> Удаляет чат (группу) из БД. </p>
     * @brief delete_chat
     * @param chat_id - Идентификатор удаляемого чата (группы).
     * @return - В случае успеха возвращает true, иначе - false.
     */
    bool delete_chat(unsigned long long chat_id);

    /**
     * <p> Удаляет пользователя из чата (группы). </p>
     * @brief quit_chat
     * @param user_id - Идентификатор удаляемого из чата пользователя.
     * @param chat_id - Идентификатор чата, из которого удаляется пользователь.
     * @return - В случае успеха возвращает true, иначе - false.
     */
    bool quit_chat(unsigned long long user_id, unsigned long long chat_id);

    /**
     * <p> Добавляет сообщение в чат (группу). </p>
     * @brief add_chat_message
     * @param msg - Добавляемое сообщение.
     * @return - В случае успеха возвращает true, иначе - false.
     */
    bool add_chat_message(const IMessage& msg);

    /**
     * <p> Удаляет сообщение из чата (группы). </p>
     * @brief delete_chat_message
     * @param msg - Удаляемое сообщение.
     * @return - В случае успеха возвращает true, иначе - false.
     */
    bool delete_chat_message(const IMessage& msg);

    /**
     * <p> Извлекает сообщения чата из БД. </p>
     * @brief get_chat_messages
     * @param chat_id - Идентификатор чата, из которого извлекаются сообщения.
     * @return - Вектор сообщений.
     */
    std::vector<std::shared_ptr<IMessage> > get_chat_messages(unsigned long long chat_id);

    /**
     * <p> Возвращает список никнеймов участников чата (группы). </p>
     * @brief get_channel_participants
     * @param group_id - Идентификатор чата (группы), из которого извлекаются участники.
     * @return - Список участников чата (группы).
     */
    std::vector<std::string> get_chat_participants(unsigned long long group_id);

    /**
     * <p> Возвращает информацию о пользователе, который соответствует никнейму. </p>
     * @brief get_user
     * @param username - Никнейм пользователя
     * @return - Структура, содержащая информацию о пользователе
     */
    StatusInfo get_status_info(const std::string &username);

    /**
     * <p> Возвращает информацию о пользователе, который соответствует идентификатору. </p>
     * @brief get_user
     * @param username - Никнейм пользователя
     * @return - Структура, содержащая информацию о пользователе
     */
    StatusInfo get_status_info(unsigned long long id);

private:
    QSqlDatabase data_base_;
    std::string connection_name_;

    /**
     * <p> Настраивает и устанавливает соединение с БД MySQL. </p>
     * @brief setup
     */
    void setup();

    /**
     * <p> Настраивает таблицы в БД. </p>
     * @brief setup_tables
     * @return - Возвращает true в случае успеха, иначе - false.
     */
    bool setup_tables();
};

namespace link_dove_database_details__ {
    /**
     * <p> Извлекает данные о группе, которые возвращает объект query. </p>
     * @brief retrieve_channel_info
     * @param query - Запрос, содержащий данные о канале.
     * @return - Структура. содержащая данные о канале.
     */
    ChannelInfo retrieve_channel_info(const QSqlQuery &query);

    /**
     * <p Извлекает данные о чате, которые возвращает объект query. </p>
     * @brief retrieve_chat_info
     * @param query - Запрос, содержащий данные о чате.
     * @return - Структура, содержащая данные о чате.
     */
    ChatInfo retrieve_chat_info(const QSqlQuery &query);

    /**
     * <p> Извлекает данные о пользователе, которые возвращает объект query. </p>
     * @brief retrieve_status_info
     * @param query - Запрос, содержащий данные о пользователе.
     * @return - Структура. содержащая данные о пользователе.
     */
    StatusInfo retrieve_status_info(const QSqlQuery &query);

    /**
     * <p> Извлекает заданное количество жалоб. </p>
     * @brief retrieve_complaints
     * @param query - Запрос, содержащий жалобы
     * @param count - Количество жалоб для извлечения
     * @return - Вектор жалоб размером размером не более count
     */
    std::vector<Complaint> retrieve_complaints(QSqlQuery& query, int count);

    /**
     * <p> Извлекает заданное количество уведомлений. </p>
     * @brief retrieve_notifications
     * @param query - Запрос, содержащий уведомления
     * @param count - Количество увдомлений для извлечения
     * @return - Вектор уведомлений размером не более count
     */
    std::vector<Notification> retrieve_notifications(QSqlQuery& query, int count);

    /**
     * <p> Извлекает список сообщений между двумя пользователями, котороый возвращает объект query. </p>
     * @brief retrieve_ind_messages
     * @param query - Запрос, содержащий список сообщений
     * @param content_query - Вспомогательный запрос для извлечения содержимого
     * @return - Список сообщений.
     */
    std::vector<std::shared_ptr<IMessage>> retrieve_ind_messages(QSqlQuery &query, QSqlQuery &content_query);

    /**
     * <p> Извлекает список сообщений из канала. </p>
     * @brief retrieve_channel_messages
     * @param query - Запрос, содержащий список сообщений
     * @param content_query - Вспомогательный запрос для извлечения содержимого
     * @return - Список сообщений.
     */
    std::vector<std::shared_ptr<IMessage>> retrieve_channel_messages(QSqlQuery &query, QSqlQuery &content_query);
}

#endif // LINKDOVESQLDATABASE_H
