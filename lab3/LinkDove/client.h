#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <memory>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

#include "IStreamConnection.h"

#define LOCAL_ADDRESS "127.0.0.1"
#define END_OF_REQUEST "\r\r\r\r\n"

namespace asio = boost::asio;
using tcp = asio::ip::tcp;

class Client final : public std::enable_shared_from_this<Client>
{
public:
    Client(std::shared_ptr<asio::io_context> io_context_ptr, boost::asio::ip::address address, uint16_t port);

    /**
     * <p> Подключает клиента к серверу. Отправляет данные для авторизации. </p>
     * @brief connect
     */
    void async_connect();

    /**
     * <p> Устанавливает данные о пользователе, которые отправляются при авторизации. </p>
     * Функция должна вызываться до функции login.
     * @brief setInfo
     * @param username - Имя пользователя.
     * @param email_ - Электронная почта пользователя.
     * @param password - Пароль пользователя.
     */
    void setInfo(const std::string& username, const std::string& email_, const std::string& password);

    /**
     * <p> Осуществляет попытку авторизации пользователя. </p>
     * @brief async_login
     */
    void async_login();

    /**
     * <p> Осуществляет попытку регистрации пользователя. </p>
     * @brief async_register
     */
    void async_register();

private:
    std::shared_ptr<asio::io_context> io_context_ptr_;
    IStreamConnection<tcp> connection_;
    tcp::endpoint endpoint_;
    bool is_connected_;

    std::string username_;
    std::string email_;
    std::string password_;

    void CreateAccount() {}

    /**
     * <p> Формирует запрос авторизации. </p>
     * @brief create_login_request
     * @return - Строка запроса.
     */
    std::string create_login_request();

    /**
     * <p> Формирует запрос регистрации. </p>
     * @brief create_register_request
     * @return - Строка запроса.
     */
    std::string create_register_request();

    /**
     * <p> Обрабатывает попытку асинхронного подключения к серверу. </p>
     * @brief handle_async_connect
     */
    void handle_async_connect(boost::system::error_code error);

    /**
     * <p> Обрабатывает попытку авторизации. </p>
     * @brief handle_async_login
     */
    void handle_async_login(boost::system::error_code error, size_t bytes_transferred);

    /**
     * <p> Обрабатывает попытку регистрации. </p>
     * @brief handle_async_login
     */
    void handle_async_register(boost::system::error_code error, size_t bytes_transferred);

    /** <p> Запускает в отдельном потоке контекст для обработки асинхронных функций. </p>
     * @brief run_context
     */
    void run_context();
};

#endif // CLIENT_H
