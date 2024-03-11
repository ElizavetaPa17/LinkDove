#ifndef LINKDOVESERVER_H
#define LINKDOVESERVER_H

#include <list>
#include <memory>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

#include "tcpconnection.h"

namespace asio = boost::asio;
using tcp  = asio::ip::tcp;

class LinkDoveServer final : public std::enable_shared_from_this<LinkDoveServer>
{
public:
    LinkDoveServer();

    /**
     * <p> Подключает сервер к порту для прослушивания.</p>
     * В случае ошибки генерирует исключение.
     * @brief listen
     * @param port Номер подключаемого порта
     */
    void listen(uint16_t port);

private:
    asio::io_context io_context_;
    tcp::acceptor acceptor_;
    std::list<TCPConnection> connections_;
    using ConnectionIterator = std::list<TCPConnection>::iterator;

    /**
     * <p> Начинает принимать соединения к порту, указанному при вызове функции listen. </p>
     * @brief start_accept
     */
    void start_accept();

    /**
     * <p> Обработчик нового соединения </p>
     * @brief handle_accept
     * @param socket - Сокет, который пытается установить соединение.
     * @param error - Параметр, содержащий ошибку в случае неудачной попытки соединения.
     */
    void handle_accept(ConnectionIterator iterator, boost::system::error_code error);

    /** <p> Запускает в отдельном потоке контекст для обработки асинхронных функций. </p>
     * @brief run_context
     */
    void run_context();

};

#endif // LINKDOVESERVER_H
