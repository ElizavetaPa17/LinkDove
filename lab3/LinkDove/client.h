#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <string>
#include <memory>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

#include <functional>

#include "IStreamConnection.h"
#include "logininfo.h"
#include "UserInfo.h"
#include "channelinfo.h"
#include "chatinfo.h"
#include "complaint.h"
#include "constants.h"
#include "notification.h"
#include "imessage.h"

namespace asio = boost::asio;
using tcp = asio::ip::tcp;

/**
 * @brief The Client class
 * Клиент, который осуществляет взаимодействие с сервером.
 */
class Client final : public QObject, public std::enable_shared_from_this<Client>
{
    Q_OBJECT
public:
    Client(std::shared_ptr<asio::io_context> io_context_ptr, boost::asio::ip::address address, uint16_t port);

    /**
     * <p> Подключает клиента к серверу. Отправляет данные для авторизации. </p>
     * @brief connect
     */
    void async_connect();

    /**
     * <p> Осуществляет попытку авторизации пользователя. </p>
     * @param login_info - Структура, содержащая информацию для прохождения авторизации.
     * @brief async_login
     */
    void async_login(const LoginInfo& login_info);

    /**
     * <p> Осуществляет попытку регистрации пользователя. </p>
     * @param register_info - Структура, содержащая информацию для прохождения регистрации.
     * @brief async_register
     */
    void async_register(const UserInfo& user_info);

    /**
     * <p> Отправляет жалобу от пользователя администратору. </p>
     * @param text - Содержание жалобы.
     * @brief async_send_complaint
     */
    void async_send_complaint(const std::string& text);

    /**
     * <p> Отправляет запрос на удаление жалобы с заданным идентификатором. </p>
     * @brief async_del_complaint
     * @param complaint_id - Идентификатор удаляемой жалобы.
     */
    void async_del_complaint(unsigned long long complaint_id);

    /**
     * <p> Получает список всех жалоб. </p>
     * @brief async_get_complaints
     */
    void async_get_complaints();

    /**
     * <p> Отправляет отредактированные данные пользователя. </p>
     * @brief async_update_user
     * @param status_info_ - Структура, содержащая отредактированные данные пользователя.
     */
    void async_update_user(StatusInfo& status_info_);

    /**
     * <p> Отправляет запрос на извлечение информации о пользователе с заданным идентификатором. </p>
     * @brief async_find_user
     * @param id - Идентификатор пользователя, поиск которого осуществляется.
     */
    void async_find_user(const std::string &username);

    /**
     * <p> Отправляет запрос на блокировку/разблокировку пользователя с никнеймом username в зависимости от флага is_ban.</p>
     * @brief async_ban_user
     * @param username - Никнейм пользователя.
     * @param is_ban - Если is_ban равно true, то пользователь блокируется, иначе - разблокировывается..
     */
    void async_ban_user(const std::string &username, bool is_ban);

    /**
     * <p> Отправляет запрос на блокировку/разблокировку пользователя в переписке. </p>
     * @brief async_ban_ind_user
     * @param username - Никнейм пользователя, которого мы блокируем/разблокируем.
     * @param is_ban - Флаг блокировки/разблокировки.
     */
    void async_ban_ind_user(const std::string &username, bool is_ban);

    //void async_ban_chnnl_user(const std::string &username, bool is_ban);

    //void async_ban_chat_user(const std::string &username, bool is_ban);

    /**
     * <p> Проверяет, не заблокирован ли текущий пользователь собеседником в переписке. </p>
     * @brief async_is_banned_ind_user
     * @param username - Никнейм пользователя, статус блокировки по отношению к которому опрашивается.
     */
    void async_is_banned_ind_user(const std::string &username);

    /**
     * </p> Отправляет запрос на получение списка заблокированных собеседников в переписках. </p>
     * @brief async_get_banned_interlocutors
     */
    void async_get_banned_interlocutors();

    /**
     * <p> Отправляет сообщение. </p>
     * @brief async_send_message
     * @param message - Отправляемое сообщение.
     */
    void async_send_message(const IMessage& message);

    /**
     * <p> Отправляет запрос на получение всех сообщений в чате с одним собеседником с идентификатором other_id. </p>
     * @brief async_get_messages
     * @param other_id - Идентификатор собеседника.
     */
    void async_get_ind_messages(unsigned long long other_id);

    /**
     * <p> Отправляет запрос на получение информации о всех собеседниках, с которыми у текущего пользователя есть активная переписка. </p>
     * @brief async_get_interlocutors
     */
    void async_get_interlocutors();

    /**
     * <p> Удаляет переписку между двумя собеседниками. </p>
     * @brief async_delete_ind_chat
     * @param interlocutor_id - Идентификатор собеседника.
     */
    void async_delete_ind_chat(unsigned long long interlocutor_id);

    /**
     * <p> Отправляет запрос на создание канала. </p>
     * @brief async_create_channel
     * @param channel_name - Название создаваемого канала.
     */
    void async_create_channel(const std::string &channel_info);

    /**
     * <p> Отправляет запрос на получение информации о канале с названием channel_name. </p>
     * @brief async_find_channel
     * @param channel_name - Название канала.
     */
    void async_find_channel(const std::string &channel_name);

    /**
     * <p> Отправляет запрос на получение информации о всех каналах, в которых состоит пользователь. </p>
     * @brief async_get_channels
     */
    void async_get_channels();

    /**
     * <p> Отправляет запрос на получение информации о том, является ли текущий пользователь участником канала. </p>
     * @brief is_channel_participant_request
     * @param channel_id - Идентификатор канала.
     */
    void async_is_channel_participant_request(unsigned long long channel_id);

    /**
     * <p> Отправляет запрос на добавление текущего пользователя в канал с идентификатором channel_id.</p>
     * @brief async_add_channel_participant_request
     * @param channel_id - Идентификатор канала, в который добавляется текущий пользователь.
     */
    void async_add_channel_participant_request(unsigned long long channel_id);

    /**
     * <p> Отправляет запрос на получение всех сообщений из канала. </p>
     * @brief async_get_channel_messages
     * @param channel_id - Идентификатор канала, из которого извлекаются сообщения.
     */
    void async_get_channel_messages(unsigned long long channel_id);

    /**
     * <p> Отправляет запрос на получение списка участников канала. </p>
     * @brief async_get_channel_participants
     * @param channel_id - Идентификатор канала, из которого извлекаются участники.
     */
    void async_get_channel_participants(unsigned long long channel_id);

    /**
     * <p> Отправляет запрос на удаление канала. </p>
     * @brief async_delete_channel
     * @param channel_id - Идентификатор удаляемого канала.
     */
    void async_delete_channel(unsigned long long channel_id);

    /**
     * <p> Отправляет запрос на удаление пользователя из канала. </p>
     * @brief async_remove_user_from_channel
     * @param channel_id - Идентификатор канала, из которого удаляется пользователь.
     * @param user_name - Никнейм пользователя, который удаляется из канала.
     */
    void async_remove_user_from_channel(unsigned long long channel_id, const std::string &user_name);

    /**
     * <p> Удаляет пользователя из канала. </p>
     * @brief async_quit_channel
     * @param user_id - Идентификатор удаляемого из канала пользователя.
     * @param channel_id - Идентификатор канала, из которого удаляется пользователь.
     */
    void async_quit_channel(unsigned long long user_id, unsigned long long channel_id);

    /**
     * <p> Отправляет запрос на создание чата. </p>
     * @brief async_create_chat
     * @param chat_name - Название чата.
     */
    void async_create_chat(const std::string &chat_name);

    /**
     * <p> Отправляет запрос на получение чатов текущего пользователя. </p>
     * @brief async_get_chats
     */
    void async_get_chats();

    /**
     * <p> Отправляет запрос на поиск информации о чате. </p>
     * @brief async_find_chat
     * @param chat_name - Название чата.
     */
    void async_find_chat(const std::string &chat_name);

    /**
     * <p> Отправляет запрос на получение информации о том, является ли текущий пользователь участником чата. </p>
     * @brief is_chat_participant_request
     * @param chat_id - Идентификатор чата.
     */
    void async_is_chat_participant_request(unsigned long long chat_id);

    /**
     * <p> Отправляет запрос на добавление текущего пользователя в чат с идентификатором chat_id.</p>
     * @brief async_add_chat_participant_request
     * @param chat_id - Идентификатор чата в который добавляется текущий пользователь.
     */
    void async_add_chat_participant_request(unsigned long long chat_id);

    /**
     * <p> Получает сообщения из чата с идентификатором chat_id. </p>
     * @brief async_get_chat_messages
     * @param chat_id - Идентификатор чата.
     */
    void async_get_chat_messages(unsigned long long chat_id);

    /**
     * <p> Отправляет запрос на получение списка участников чата (группы). </p>
     * @brief async_get_chat_participants
     * @param group_id - Идентификатор чата (группы), из которого извлекается список участников.
     */
    void async_get_chat_participants(unsigned long long group_id);

    /**
     * <p> Отправляет запрос на удаление чата (группы). </p>
     * @brief async_delete_chat
     * @param channel_id - Идентификатор удаляемой группы.
     */
    void async_delete_chat(unsigned long long group_id);

    /**
     * <p> Отправляет запрос на удаление пользователя из группы. </p>
     * @brief async_quit_chat
     * @param user_id - Идентификатор удаляемого пользователя.
     * @param group_id - Идентификатор группы, из которой удаляется пользователь.
     */
    void async_quit_chat(unsigned long long user_id, unsigned long long group_id);

    /**
     * <p> Отправляет запрос на удаление пользователя из чата (группы) владельцем чата (группы). </p>
     * @brief async_remove_user_from_chat
     * @param group_id - Идентификатор чата (группы), из которого удаляется пользователь.
     * @param user_name - Никнейм пользователя, который удаляется.
     */
    void async_remove_user_from_chat(unsigned long long group_id, const std::string &user_name);

    /**
     * <p> Отправляет запрос на отправку ответа на жалобу пользователю. </p>
     * @brief async_answer_user
     * @param user_id - Идентификатор пользователя, которому отправляется жалоба.
     * @param text - Сообщение ответа.
     */
    void async_answer_user(unsigned long long user_id, const std::string &text);

    /**
     * <p> Отправляет запрос на получение уведомлений от администратора. </p>
     * @brief async_get_notifications
     */
    void async_get_notifications();

    /**
     * <p> Отправляет запрос на удаление уведомления. </p>
     * @brief async_del_notification
     * @param id - Идентификатор удаляемого уведомления.
     */
    void async_del_notification(unsigned long long id);

    /**
     * <p> Отправляет запрос на удаление сообщения. </p>
     * @brief async_delete_msg
     * @param msg - Удаляемое сообщение.
     */
    void async_delete_msg(const IMessage &msg);

    /**
     * <p> Возвращает информацию о пользователе. </p>
     * @brief get_status_info
     * @return - Структура, содержащая информацию о пользователе.
     */
    StatusInfo get_status_info();

    /**
     * <p> Возвращает информацию о найденном пользователе. Вызывается после запроса на поиск пользователя. </p>
     * @brief get_found_user
     * @return - Структура, содержащая информацию о найденном пользователе.
     */
    StatusInfo get_found_user();

    /**
     * <p> Возвращает информацию о найденном канале. Вызывает после запроса на поиск канала. </p>
     * @brief get_found_channel
     * @return - Структура, содержащая информацию о найденном канале.
     */
    ChannelInfo get_found_channel();

    /**
     * <p> Возвращает информацию о найденном чате. </p>
     * @brief get_found_chat
     * @return - Структура, содержащая информацию о найденном чате.
     */
    ChatInfo get_found_chat();

    /**
     * <p> Определяет, установил ли клиент соединение с сервером. </p>
     * @brief isConnected
     * @return - Возвращает true, если клиент установил соединение с сервером, иначе - false.
     */
    bool is_connected() noexcept;

signals:
    /**
     * <p> Генерирует сигнал после получения результата авторизации. </p>
     * @brief authorization_result
     * @param result - Параметр, содержащий результат авторизации.
     */
    void authorization_result(int result);

    /**
     * <p> Генерирует сигнал после получения результата отправления жалобы. </p>
     * @brief complaint_result
     * @param result - Параметр, содержащий результат отправления жалобы.
     */
    void send_complaint_result(int result);

    /** <p> Генерирует сигнал после получения результата удаления жалобы. </p>
     * @brief del_complaint_result
     * @param result - Параметр, содержащий результат удаления жалобы.
     */
    void del_complaint_result(int result);

    /**
     * <p> Генерирует сигнал после получения результата извлечения всех жалоб.</p>
     * @brief get_complaints_result
     * @param result - Параметр, содержащий результат получения списка жалоб.
     * @param complaints - Список жалоб.
     */
    void get_complaints_result(int result, std::vector<Complaint> complaints);

    /**
     * <p> Генерирует сигнал после получения результата обновления данных о пользователе. </p>
     * @brief update_user_result
     * @param result - Параметр, содержащий результат обновления данных о пользователе.
     */
    void update_user_result(int result);

    /**
     * <p> Генерирует сигнал после получение результата поиска данных о пользователе. </p>
     * @brief find_user_result
     * @param result - Параметр, содержащий результат поиска пользователя.
     */
    void find_user_result(int result);

    /**
     * <p> Генерирует сигнал после получения результата блокировки/разблокировки пользователя. </p>
     * @brief ban_user_result
     * @param result - Параметр, содержащий результат блокировки/разблокировки пользователя.
     */
    void ban_user_result(int result);

    /**
     * <p> Генерирует сигнал после получения статуса блокировки пользователя. </p>
     * @brief is_banned_user_result
     * @param result - Параметр, содержащий результат получения статуса блокировки пользователя.
     * @param is_banned - Флаг статуса блокировки.
     */
    void is_banned_user_result(int result, bool is_banned);

    /**
     * <p> Генерирует сигнал после получения списка заблокированных пользователей. </p>
     * @brief get_banned_users
     * @param result - Параметр, содержащий результат получения списка.
     * @param usernames - Список никнеймов заблокированных пользователей.
     */
    void get_banned_users(int result, std::vector<std::string> usernames);

    /**
     * <p> Генерирует сигнал после получения результата отправки сообщения. </p>
     * @brief send_msg_result
     * @param result - Параметр, содержащий результат отправки сообщения.
     */
    void send_msg_result(int result);

    /**
     * <p> Генерирует сигнал после получения результата запроса на список сообщений. </p>
     * @brief get_msg_result
     * @param result - Параметр, содержащий результат отправки сообщения.
     * @param messages - Список сообщений
     */
    void get_ind_msg_result(int result, std::vector<std::shared_ptr<IMessage>> messages);

    /**
     * <p> Генерирует сигнал после получение результата запроса на информацию о собеседниках. </p>
     * @brief get_interlocutors_result
     * @param result - Параметр, содержащий результат запроса.
     * @param interlocutors - Список собеседников
     */
    void get_interlocutors_result(int result, std::vector<StatusInfo> interlocutors);

    /**
     * <p> Генерирует сигнал после получение результата запроса на удаление чата между собеседниками. </p>
     * @brief delete_ind_chat_result
     * @param result - Параметр, содержащий результат запроса.
     */
    void delete_ind_chat_result(int result);

    /**
     * <p> Генерирует сигнал после получения результата запроса на создание канала. </p>
     * @brief get_create_channel_result
     * @param result - Параметр, содержащий результат запроса.
     */
    void create_channel_result(int result);

    /**
     * <p> Генерирует сигнал после получения результата запроса на поиск информации о пользователе. </p>
     * @brief find_channel_result
     * @param result - Параметр, содержащий результат запроса.
     */
    void find_channel_result(int result);

    /**
     * <p> Генерирует сигнал после получения результата запроса информации о каналах, в которых состоит текущий пользователь. </p>
     * @brief get_channels_result
     * @param result - Параметр, содержащий результат запроса.
     * @param channels - Список каналов
     */
    void get_channels_result(int result, std::vector<ChannelInfo> channels);

    /**
     * <p> Генерирует сигнал после получения результата запроса информации о том, является ли текущий пользователь участником канала. </p>
     * @brief is_channel_participant_result
     * @param result - Параметр, содеращий результат запроса.
     * @param is_participant - В случае успеха содержит является ответом на запрос, иначе - всегда false.
     */
    void is_channel_participant_result(int result, bool is_participant);

    /**
     * <p> Генерирует сигнал после получения результата запроса на добавление текущего пользователя в канал. </p>
     * @brief add_participant_to_channel_result
     * @param result - Параметр, содеращий результат запроса.
     */
    void add_participant_to_channel_result(int result);

    /**
     * <p> Генерирует сигнал после получение результата запроса на получение из канала. </p>
     * @brief get_chnnl_messages_result
     * @param result - Параметр, содеращий результат запроса.
     * @param messages - Список сообщений
     */
    void get_channel_msg_result(int result, std::vector<std::shared_ptr<IMessage>> messages);

    /**
     * <p> Генерирует сигнал после получения результата запроса на получение списка участников канала. </p>
     * @brief get_channel_participants_result
     * @param result - Параметр, содеращий результат запроса.
     * @param participants - Список участников канала.
     */
    void get_channel_participants_result(int result, std::vector<std::string> participants);

    /**
     * <p> Генерирует сигнал после получения результата удаления канала. </p>
     * @brief delete_channel_result
     * @param result - Параметр, содеращий результат запроса.
     */
    void delete_channel_result(int result);

    /**
     * <p> Генерирует сигнал после получения результата удаления пользователя из канала. </p>
     * @brief quit_channel_result
     * @param result - Параметр, содеращий результат запроса.
     */
    void quit_channel_result(int result);

    /**
     * <p> Генерирует сигнал после получения результата удаления пользователя из канала владельцем канала. </p>
     * @brief remove_user_from_channel_result
     * @param result - Параметр, содеращий результат запроса.
     */
    void remove_user_from_channel_result(int result);

    /**
     * <p> Генерирует сигнал после получения результата запроса на создание чата.  </p>
     * @brief create_chat_result
     * @param result - Параметр, содержащий результат запроса.
     */
    void create_chat_result(int result);

    /**
     * <p> Генерирует сигнал после получения результата запроса на поиск информации о чатах, в которых состоит текущий пользователь. </p>
     * @brief get_chats_result
     * @param result - Параметр, содержащий результат запроса.
     * @param chats - Список чатов
     */
    void get_chats_result(int result, std::vector<ChatInfo> chats);

    /**
     * <p> Генерирует сигнал после получения результата поиска информации о чате. </p>
     * @brief find_chat_result
     * @param result - Параметр, содержащий результат запроса.
     */
    void find_chat_result(int result);

    /**
     * <p> Генерирует сигнал после получения результата запроса информации о том, является ли текущий пользователь участником чата. </p>
     * @brief is_chat_participant_result
     * @param result - Параметр, содеращий результат запроса.
     * @param is_participant - В случае успеха содержит является ответом на запрос, иначе - всегда false.
     */
    void is_chat_participant_result(int result, bool is_participant);

    /**
     * <p> Генерирует сигнал после получения результата запроса на добавление текущего пользователя в чат. </p>
     * @brief add_participant_to_chat_result
     * @param result - Параметр, содеращий результат запроса.
     */
    void add_participant_to_chat_result(int result);

    /**
     * <p> Генерирует сигнал после получения результата запроса на получение списка сообщений из чата (группы). </p>
     * @brief get_chat_msg_result
     * @param result - Параметр, содержащий результат запроса.
     * @param messages - Список сообщений.
     */
    void get_chat_msg_result(int result, std::vector<std::shared_ptr<IMessage>> messages);

    /**
     * <p> Генерирует сигнал после получения результата запроса на получение списка участников чата (группы). </p>
     * @brief get_chat_participants_result
     * @param result - Параметр, содержащий результат запроса.
     * @param participants - Список участников чата (группы).
     */
    void get_chat_participants_result(int result, std::vector<std::string> participants);

    /**
     * <p> Генерирует сигнал после получения результата запроса на удаление пользователя из чата (группы). </p>
     * @brief quit_chat_result
     * @param result - Параметр, содержащий результат запроса.
     */
    void quit_chat_result(int result);

    /**
     * <p> Генерирует сигнал после получения результата удаления чата (группы). </p>
     * @brief delete_chat_result
     * @param result - Параметр, содеращий результат запроса.
     */
    void delete_chat_result(int result);

    /**
     * <p> Генерирует сигнал после получения результата удаления пользователя из чата владельцем чата. </p>
     * @brief remove_user_from_chat_result
     * @param result - Параметр, содержащий результат запроса.
     */
    void remove_user_from_chat_result(int result);

    /**
     * <p> Генерирует сигнал после получения результата отправки ответа на жалобу пользователя. </p>
     * @brief answer_user_result
     * @param result - Параметр, содержащий результат запроса.
     */
    void answer_user_result(int result);

    /**
     * <p> Генерирует сигнал после получения результата получения списка уведомлений. </p>
     * @brief get_notifications_result
     * @param result - Параметр, содержащий результат запроса.
     * @param notifications - Список уведомлений.
     */
    void get_notifications_result(int result, std::vector<Notification> notifications);

    /**
     * <p> Генерирует сигнал после получение результата запроса на удаление уведомления. </p>
     * @brief del_notification_result
     * @param result - Параметр, содержащий результат запроса.
     */
    void del_notification_result(int result);

    /**
     * <p> Генерирует сигнал после получения результата удаления сообщения. </p>
     * @brief delete_msg_result
     * @param result - Параметр, содержащий результат запроса.
     */
    void delete_msg_result(int result);

private:
    std::shared_ptr<asio::io_context> io_context_ptr_;
    IStreamConnection<tcp> connection_;
    tcp::endpoint endpoint_;
    bool is_connected_;
    std::unordered_map<std::string, std::function<void()>> response_tree_;

    StatusInfo status_info_;
    StatusInfo updated_status_info_;
    StatusInfo found_status_info_;
    ChannelInfo found_channel_info_;
    ChatInfo found_chat_info_;

    /**
     * <p> Настраивает дерево обработки ответов от сервера. </p>
     * @brief setup_response_tree
     */
    void setup_response_tree();

    /**
     * <p> Асинхронно читает ответ от сервера. </p>
     * @brief async_read
     */
    void async_read();

    /**
     * <p> Обрабатывает попытку асинхронного подключения к серверу. </p>
     * @brief handle_async_connect
     */
    void handle_async_connect(boost::system::error_code error);

    /**
     * <p> Обработчик асинхронной функции записи. </p>
     * @brief handle_async_login
     * @param error - Параметр, содержащий ошибку в случае неудачной записи в сокет.
     * @param bytes_transfered - Количество записанных байтов.
     */
    void handle_async_write(boost::system::error_code error, size_t bytes_transferred);

    /**
     * <p> Обрабатывает попытку чтения ответа от сервера. </p>
     * @brief handle_async_write
     * @param error - Параметр, содержащий ошибку в случае неудачного чтения из сокета.
     * @param bytes_transfered - Количество прочитанных байтов.
     */
    void handle_async_read(boost::system::error_code error, size_t bytes_transferred);

    /**
     * <p> Отправляет серверу запрос. </p>
     * @brief write_to_server
     */
    void write_to_server();

    /** <p> Запускает в отдельном потоке контекст для обработки асинхронных функций. </p>
     * @brief run_context
     */
    void run_context();

    /**
     * <p> Удаляет разделитель END_OF_REQUEST из буфера сокета. </p>
     * @brief remove_delimeter
     */
    void remove_delimeter();
};

#endif // CLIENT_H
