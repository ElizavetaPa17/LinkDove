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
#include "complaint.h"
#include "constants.h"

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
     * @param complaint - Структура, содержащая информацию о жалобе
     * @brief async_send_complaint
     */
    void async_send_complaint(const Complaint& complaint);

    /**
     * <p> Возвращает информацию о пользователе. </p>
     * @brief get_status_info
     * @return - Структура, содержащая информацию о пользователе
     */
    StatusInfo get_status_info();

    /**
     * <p> Определяет, установил ли клиент соединение с сервером. </p>
     * @brief isConnected
     * @return - Возвращает true, если клиент установил соединение с сервером, иначе - false.
     */
    bool is_connected() noexcept;

signals:
    void authorization_result(int result);
    void complaint_result(int result);

private:
    std::shared_ptr<asio::io_context> io_context_ptr_;
    IStreamConnection<tcp> connection_;
    tcp::endpoint endpoint_;
    bool is_connected_;

    StatusInfo status_info_;

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
     * <p> Обрабатывает попытку авторизации. </p>
     * @brief handle_async_login
     * @param error - Параметр, содержащий ошибку в случае неудачной записи в сокет.
     * @param bytes_transfered - Количество записанных байтов.
     */
    void handle_async_login(boost::system::error_code error, size_t bytes_transferred);

    /**
     * <p> Обрабатывает попытку регистрации. </p>
     * @brief handle_async_login
     * @param error - Параметр, содержащий ошибку в случае неудачной записи в сокет.
     * @param bytes_transfered - Количество записанных байтов.
     */
    void handle_async_register(boost::system::error_code error, size_t bytes_transferred);

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
