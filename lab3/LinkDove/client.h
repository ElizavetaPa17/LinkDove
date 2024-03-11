#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <memory>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

#include "IStreamConnection.h"

#define LOCAL_ADDRESS "127.0.0.1"

namespace asio = boost::asio;
using tcp = asio::ip::tcp;

class Client final : public std::enable_shared_from_this<Client>
{
public:
    Client(std::shared_ptr<asio::io_context> io_context_ptr, boost::asio::ip::address address, uint16_t port);

    /**
     * <p> Подключает клиента к серверу. </p>
     * @brief connect
     */
    void async_connect();

    /**
     * <p> Осуществляет попытку авторизации пользователя. </p>
     * @brief async_login
     */
    void async_login();

    /**
     * <p> Устанавливает данные о пользователе, которые отправляются при авторизации. </p>
     * Функция должна вызываться до функции login.
     * @brief setInfo
     * @param username - Имя пользователя.
     * @param email_ - Электронная почта пользователя.
     * @param password - Пароль пользователя.
     */
    void setInfo(const std::string& username, const std::string& email_, const std::string& password);

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
     * Формирует запрос авторизации.
     * @brief create_login_request
     * @return - Строка запроса.
     */
    std::string create_login_request();

    /**
     * <p> Обрабатывает попытку асинхронного подключения к серверу. </p>
     * @brief on_async_connect
     */
    void on_async_connect(boost::system::error_code error);
    /**
     * <p> Обрабатывает попытку авторизации. </p>
     * @brief on_async_login
     */
    void on_async_login(boost::system::error_code error, size_t bytes_transferred);

    /** <p> Запускает в отдельном потоке контекст для обработки асинхронных функций. </p>
     * @brief run_context
     */
    void run_context();
};

#endif // CLIENT_H
