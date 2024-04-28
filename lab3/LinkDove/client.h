#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <string>
#include <memory>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

#include "IStreamConnection.h"
#include "logininfo.h"
#include "UserInfo.h"
#include "channelinfo.h"
#include "complaint.h"
#include "constants.h"
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
     * <p> Возвращает вектор жалоб. Вызывается после запроса на получение списка жалоб (в противном случае результат не определен). </p>
     * @brief get_complaints
     * @return - Вектор жалоб.
     */
    std::vector<Complaint> get_complaints();

    /**
     * <p> Возвращает вектор сообщений. Вызывается после запроса на получение списка жалоб. </p>
     * @brief get_messages
     * @return - Вектор сообщений.
     */
    std::vector<std::shared_ptr<IMessage>> get_messages();

    /**
     * <p> Возвращает вектор собеседников текущего пользователя. Вызывается после запроса на получение этих собеседников. </p>
     * @brief get_interlocutors
     * @return - Вектор собеседников текущего пользователя.
     */
    std::vector<StatusInfo> get_interlocutors();

    /**
     * <p> Возвращает информацию о найденном канале. Вызывает после запроса на поиск канала. </p>
     * @brief get_found_channel
     * @return - Структура, содержащая информацию о найденном канале.
     */
    ChannelInfo get_found_channel();

    /**
     * <p> Возвращает вектор каналов, в которых состоит текущий пользователь. Вызывается после запроса на получение этих каналов. </p>
     * @brief get_channels
     * @return - Вектор каналов, в которых состоит текущий пользователь.
     */
    std::vector<ChannelInfo> get_channels();

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
     */
    void get_complaints_result(int result);

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
     * <p> Генерирует сигнал после получения результата отправки сообщения. </p>
     * @brief send_msg_result
     * @param result - Параметр, содержащий результат отправки сообщения.
     */
    void send_msg_result(int result);

    /**
     * <p> Генерирует сигнал после получения результата запроса на список сообщений. </p>
     * @brief get_msg_result
     * @param result
     */
    void get_ind_msg_result(int result);

    /**
     * <p> Генерирует сигнал после получение результата запроса на информацию о собеседниках. </p>
     * @brief get_interlocutors_result
     * @param result - Параметр, содержащий результат запроса.
     */
    void get_interlocutors_result(int result);

    /**
     * <p> Генерирует сигнал после получения результата запроса на создание канала. </p>
     * @brief get_create_channel_result
     * @param result - Параметр, содержащий результат запроса.
     */
    void get_create_channel_result(int result);

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
     */
    void get_channels_result(int result);

private:
    std::shared_ptr<asio::io_context> io_context_ptr_;
    IStreamConnection<tcp> connection_;
    tcp::endpoint endpoint_;
    bool is_connected_;

    StatusInfo status_info_;
    StatusInfo updated_status_info_;
    StatusInfo found_status_info_;
    ChannelInfo found_channel_info_;

    std::vector<Complaint> complaints_;
    std::vector<std::shared_ptr<IMessage>> messages_;
    std::vector<StatusInfo> status_info_vec_;
    std::vector<ChannelInfo> channels_;

    void create_account() {}

    /**
     * <p> Формирует запрос авторизации. </p>
     * @brief create_login_request
     * @param login_info - Структура, содержащая информацию для прохождения авторизации.
     * @return - Строковое представление запроса.
     */
    std::string create_login_request(const LoginInfo& login_info);

    /**
     * <p> Формирует запрос регистрации. </p>
     * @brief create_register_request
     * @return - Строковое представление запроса.
     */
    std::string create_register_request(const UserInfo& user_info);

    /**
     * <p> Формирует запрос отправки жалобы от обычного пользователя администратору. </p>
     * @brief create_send_complaint_request
     * @param complaint - Структура, содержащая информацию о жалобе
     * @return - Строковое представление запроса.
     */
    std::string create_send_complaint_request(const Complaint& complaint);

    /**
     * <p> Формирует запрос на удаление жалобы. </p>
     * @brief create_del_request
     * @param complaint_id - Идентификатор удаляемой жалобы.
     * @return - Строковое представление запроса.
     */
    std::string create_del_request(unsigned long long complaint_id);

    /**
     * <p> Формирует запрос обновления данных о пользователе. </p>
     * @brief create_update_user_request
     * @param status_info - Структура, содержащая отредактированные данные пользователя.
     * @return - Строковое представление запроса.
     */
    std::string create_update_user_request(const StatusInfo& status_info);

    /**
     * Формирует запрос на блокировку/разблокировку пользователя.
     * @brief create_ban_user_request
     * @param username - Никнейм пользователя.
     * @param is_ban - Флаг блокировки/разблокировки пользователя.
     * @return - Строковое представление запроса.
     */
    std::string create_ban_user_request(const std::string &username, bool is_ban);

    /**
     * <p> Формирует запрос на получение сообщений из чата с одним собеседником с идентификатором other_id. </p>
     * @brief create_get_message_request
     * @param other_id - Идентификатор собеседника.
     * @return - Строковое представление запроса.
     */
    std::string create_get_ind_message_request(unsigned long long other_id);

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
