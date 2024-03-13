#ifndef LINKDOVESERVER_H
#define LINKDOVESERVER_H

#include <list>
#include <memory>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

#include "tcpconnection.h"
#include "linkdovesqldatabase.h"

#define END_OF_REQUEST "\r\r\r\r\n"

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
     * @param error - Парааметр, содержащий ошибку в случае неудачного чтения из сокета.
     * @param bytes_transfered - Количество переданных байтов.
     */
    void handle_async_read(ConnectionIterator iterator, boost::system::error_code error, size_t bytes_transfered);

    /** <p> Запускает в отдельном потоке контекст для обработки асинхронных функций. </p>
     * @brief run_context
     */
    void run_context();

};

#endif // LINKDOVESERVER_H
