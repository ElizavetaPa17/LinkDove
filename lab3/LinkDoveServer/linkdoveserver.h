#ifndef LINKDOVESERVER_H
#define LINKDOVESERVER_H

#include <list>
#include <memory>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

#include "tcpconnection.h"
#include "linkdovesqldatabase.h"
#include "constants.h"

namespace asio = boost::asio;
using tcp  = asio::ip::tcp;

class LinkDoveServer final : public std::enable_shared_from_this<LinkDoveServer>
{
public:
    LinkDoveServer();
    ~LinkDoveServer();

    /**
     * <p> Подключает сервер к порту для прослушивания.</p>
     * В случае ошибки генерирует исключение.
     * @brief listen
     * @param port Номер подключаемого порта
     */
    void listen(uint16_t port);

private:
    using ConnectionIterator = std::list<TCPConnection>::iterator;

    std::shared_ptr<asio::io_context> io_context_ptr_;
    tcp::acceptor acceptor_;
    std::list<TCPConnection> connections_;
    LinkDoveSQLDataBase data_base_;

    /**
     * <p> Начинает принимать соединения к порту, указанному при вызове функции listen. </p>
     * @brief start_async_accept
     */
    void start_async_accept();

    /**
     * <p> Асинхронно читает запросы, посылаемые клиентами. </p>
     * @brief async_read
     * @param iterator - Итератор на сокет, от которого ожидается ввод.
     */
    void async_read(ConnectionIterator iterator);

    /**
     * <p> Асинхронно отправляет ответы клиентам. </p>
     * @brief async_write
     * @param iterator - Итератор на сокет, которому отправляется ответ.
     */
    void async_write(ConnectionIterator iterator);

    /**
     * <p> Асинхронно отправляет ответ закрытия соединения клиенту. </p>
     * @brief async_close_write
     * @param iterator - Итератор на сокет, которому отправляется ответ.
     */
    void async_close_write(ConnectionIterator iterator);

    /**
     * <p> Обработчик нового соединения. </p>
     * @brief handle_async_accept
     * @param iterator - Итератор на сокет, который пытается установить соединение.
     * @param error - Параметр, содержащий ошибку в случае неудачной попытки соединения.
     */
    void handle_async_accept(ConnectionIterator iterator, boost::system::error_code error);

    /**
     * <p> Обработчик чтения из сокета клиента. </p>
     * @brief handle_async_read
     * @param iterator - Итератор на сокет, из которого производится чтение.
     * @param error - Параметр, содержащий ошибку в случае неудачного чтения из сокета.
     * @param bytes_transfered - Количество прочитанных байтов.
     */
    void handle_async_read(ConnectionIterator iterator, boost::system::error_code error, size_t bytes_transfered);

    /**
     * <p> Обработчик записи в сокет клиента. </p>
     * @brief handle_async_write
     * @param iterator - Итератор на сокет, к который производится запись.
     * @param error - Параметр, содержащий ошибку в случае неудачной записи в сокет.
     * @param bytes_transfered - Количество записанных байтов.
     */
    void handle_async_write(ConnectionIterator iterator, boost::system::error_code error, size_t bytes_transfered);

    /**
     * <p> Обработчик отправки в сокет клиента запроса на закрытие соединения. </p>
     * @brief handle_async_write
     * @param iterator - Итератор на сокет, в который отправлялся запрос.
     * @param error - Параметр, содержащий ошибку в случае неудачной записи в сокет.
     * @param bytes_transfered - Количество записанных байтов.
     */
    void handle_async_close_write(ConnectionIterator iterator, boost::system::error_code error, size_t bytes_transfered);

    /**
     * <p> Обрабатывает запрос клиента. Выводит тип запроса и вызывает специализированный обработчик. </p>
     * @brief handle_request
     * @param iterator - Итератор на сокет, запрос которого обрабатывается.
     */
    void handle_type_request(ConnectionIterator iterator);

    /**
     * <p> Обрабатывает запрос клиента на авторизацию. </p>
     * @brief handle_login_request
     * @param iterator - Итератор на сокет, запрос которого обрабатывается.
     */
    void handle_login_request(ConnectionIterator iterator);

    /**
     * <p> Обрабатывает запрос клиента на регистрацию. </p>
     * @brief handle_register_request
     * @param iterator - Итератор на сокет, запрос которого обрабатывается.
     */
    void handle_register_request(ConnectionIterator iterator);

    /**
     * <p> Обрабатывает запрос клиента на отправку жалобы администратору. </p>
     * @brief handle_complaint_request
     * @param iterator - Итератор на сокет, запрос которого обрабатывается.
     */
    void handle_send_complaint_request(ConnectionIterator iterator);

    /**
     * <p> Обрабатывает запрос на удаление жалобы. </p>
     * @brief handle_del_complaint_request
     * @param iterator - Итератор на сокет, запрос которого обрабатывается.
     */
    void handle_del_complaint_request(ConnectionIterator iterator);

    /**
     * <p> Обрабатывает запрос на получение жалоб. </p>
     * @brief handle_get_complaints_request
     * @param iterator - Итератор на сокет, запрос которого обрабатывается.
     */
    void handle_get_complaints_request(ConnectionIterator iterator);

    /**
     * <p> Обрабатывает запрос клиента на обновление информации о пользователе. </p>
     * @brief handle_update_user_request
     * @param iterator - Итератор на сокет, запрос которого обрабатывается.
     */
    void handle_update_user_request(ConnectionIterator iterator);

    /**
     * <p> Обрабатывает запрос на поиск пользователя. </p>
     * @brief handle_find_user_request
     * @param iterator - Итератор на сокет, запрос которого обрабатывается.
     */
    void handle_find_user_request(ConnectionIterator iterator);

    /**
     * <p> Обрабатывает запрос на блокировку пользователя. </p>
     * @brief handle_ban_user_request
     * @param iterator - Итератор на сокет, запрос которого обрабатывается.
     */
    void handle_ban_user_request(ConnectionIterator iterator);

    /**
     * <p> Обрабатывает запрос на отправку сообщения. </p>
     * @brief handle_send_msg_request
     * @param iterator - Итератор на сокет, запрос которого обрабатывается.
     */
    void handle_send_msg_request(ConnectionIterator iterator);

    /**
     * <p> Обрабатывает запрос на получение списка сообщений с одним собеседником. </p>
     * @brief handle_get_msg_request
     * @param iterator - Итератор на сокет, запрос которого обрабатывается.
     */
    void handle_get_msg_request(ConnectionIterator iterator);

    /**
     * <p> Обрабатывает запрос на получение списка собеседников пользователя. </p>
     * @brief handle_get_interlocutors_request
     * @param iterator - Итератор на сокет, запрос которого обрабатывается.
     */
    void handle_get_interlocutors_request(ConnectionIterator iterator);

    /**
     * <p> Обрабатывает запрос на удаление переписки между двумя собеседниками. </p>
     * @brief handle_delete_ind_chat
     * @param iterator - Итератор на сокет, запрос которого обрабатывается.
     */
    void handle_delete_ind_chat(ConnectionIterator iterator);

    /**
     * <p> Обрабатывает запрос на создание канала. </p>
     * @brief handle_channel_create_request
     * @param iterator - Итератор на сокет, запрос которого обрабатывается.
     */
    void handle_channel_create_request(ConnectionIterator iterator);

    /**
     * <p> Обрабатывает запрос на поиск информации о канале. </p>
     * @brief handle_find_channel_request
     * @param iterator - Итератор на сокет, запрос которого обрабатывается.
     */
    void handle_find_channel_request(ConnectionIterator iterator);

    /**
     * <p> Обрабатывает запрос на получение каналов, в которых состоит пользователь. </p>
     * @brief handle_get_channels_request
     * @param iterator - Итератор на сокет, запрос которого обрабатывается.
     */
    void handle_get_channels_request(ConnectionIterator iterator);

    /**
     * <p> Обрабатывает запрос на получение информации о том, является ли пользователь участником канала. </p>
     * @brief handle_is_channel_participant_request
     * @param iterator - Итератор на сокет, запрос которого обрабатывается.
     */
    void handle_is_channel_participant_request(ConnectionIterator iterator);

    /**
     * <p> Обрабатывает запрос на добавление пользователя в канал. </p>
     * @brief handle_add_channel_participant
     * @param iterator - Итератор на сокет, запрос которого обрабатывается.
     */
    void handle_add_channel_participant_request(ConnectionIterator iterator);

    /**
     * <p> Обрабатывает запрос на получение сообщений из канала. </p>
     * @brief handle_get_channel_messages
     * @param iterator - Итератор на сокет, запрос которого обрабатывается.
     */
    void handle_get_channel_messages_request(ConnectionIterator iterator);

    /**
     * <p> Обрабатывает запрос на получение списка участников канала. </p>
     * @brief handle_get_channel_participants_request
     * @param iterator - Итератор на сокет, запрос которого обрабатывается.
     */
    void handle_get_channel_participants_request(ConnectionIterator iterator);

    /**
     * <p> Обрабатывает запрос на удаление канала. </p>
     * @brief handle_delete_channel
     * @param iterator
     */
    void handle_delete_channel(ConnectionIterator iterator);

    /**
     * <p> Обрабатывает запрос на удаление пользователя из канала. </p>
     * @brief handle_quit_channel
     * @param iterator - Итератор на сокет, запрос которого обрабатывается.
     */
    void handle_quit_channel(ConnectionIterator iterator);

    /**
     * <p> Обрабатывает запрос на удаление пользователя из канала владельцем канала. </p>
     * @brief handle_remove_user_from_channel
     * @param iterator - Итератор на сокет, запрос которого обрабатывается.
     */
    void handle_remove_user_from_channel(ConnectionIterator iterator);

    /**
     * <p> Обрабатывает запрос на создание чата. </p>
     * @brief handle_create_chat_request
     * @param iterator - Итератор на сокет, запрос которого обрабатывается.
     */
    void handle_create_chat_request(ConnectionIterator iterator);

    /**
     * <p> Обрабатывает запрос на получение списка чатов, в которых состоит текущий пользователь. </p>
     * @brief handle_get_chats_request
     * @param iterator - Итератор на сокет, запрос которого обрабатывается.
     */
    void handle_get_chats_request(ConnectionIterator iterator);

    /**
     * <p> Обрабатывает запрос на поиск информации о чате. </p>
     * @brief handle_find_chat_request
     * @param iterator - Итератор на сокет, запрос которого обрабатывается.
     */
    void handle_find_chat_request(ConnectionIterator iterator);

    /**
     * <p> Обрабатывает запрос на получение информации о том, является ли пользователь участником чата. </p>
     * @brief handle_is_chat_participant_request
     * @param iterator - Итератор на сокет, запрос которого обрабатывается.
     */
    void handle_is_chat_participant_request(ConnectionIterator iterator);

    /**
     * <p> Обрабатывает запрос на добавление пользователя в чат. </p>
     * @brief handle_add_chat_participant
     * @param iterator - Итератор на сокет, запрос которого обрабатывается.
     */
    void handle_add_chat_participant_request(ConnectionIterator iterator);

    /**
     * <p> Обрабатывает запрос на получение сообщений из чата. </p>
     * @brief handle_get_chat_messages
     * @param iterator - Итератор на сокет, запрос которого обрабатывается.
     */
    void handle_get_chat_messages_request(ConnectionIterator iterator);

    /**
     * <p> Обрабатывает запрос на получение списка сообщений из чата (группы). </p>
     * @brief handle_get_chat_participants_request
     * @param iterator - Итератор на сокет, запрос которого обрабатывается.
     */
    void handle_get_chat_participants_request(ConnectionIterator iterator);

    /**
     * <p> Обрабатывает запрос на удаление чата (группы). </p>
     * @brief handle_delete_chat
     * @param iterator - Итератор на сокет, запрос которого обрабатывается.
     */
    void handle_delete_chat(ConnectionIterator iterator);

    /**
     * <p> Обрабатывает запрос на удаление пользователя из чата (группы). </p>
     * @brief handle_quit_chat
     * @param iterator - Итератор на сокет, запрос которого обрабатывается.
     */
    void handle_quit_chat(ConnectionIterator iterator);

    /**
     * <P> Обрабатывает запрос на удаление пользователя из чата (группы) владельцем чата (группы). </p>
     * @brief handle_remove_user_from_chat
     * @param iterator - Итератор на сокет, запрос которого обрабатывается.
     */
    void handle_remove_user_from_chat(ConnectionIterator iterator);

    /**
     * <p> Обрабатывает запрос на отправку ответа на жалобу пользователю. </p>
     * @brief handle_answer_user
     * @param iterator - Итератор на сокет, запрос которого обрабатывается.
     */
    void handle_answer_user(ConnectionIterator iterator);

    /**
     * <p> Обрабатывает запрос на получение уведомлений. </p>
     * @brief handle_get_notifications
     * @param iterator - Итератор на сокет, запрос которого обрабатывается.
     */
    void handle_get_notifications(ConnectionIterator iterator);

    /**
     * <p> Обрабатывает запрос на удаление уведомления. </p>
     * @brief handle_del_notification
     * @param iterator - Итератор на сокет, запрос которого обрабатывается.
     */
    void handle_del_notification(ConnectionIterator iterator);

    /** <p> Запускает в отдельном потоке контекст для обработки асинхронных функций. </p>
     * @brief run_context
     */
    void run_context();

    /**
     * <p> Удаляет разделитель END_OF_REQUEST из буфера сокета. </p>
     * @brief remove_delimeter
     * @param iterator - Итератор на сокет, из буфера которого мы удаляем разделитель.
     */
    void remove_delimeter(ConnectionIterator iterator);
};

#endif // LINKDOVESERVER_H
