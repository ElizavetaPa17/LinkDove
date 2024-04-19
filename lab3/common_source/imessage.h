#ifndef IMESSAGE_H
#define IMESSAGE_H

#include "constants.h"
#include "imessagecontent.h"
#include <memory>

class IMessage {
public:
    IMessage(unsigned long long id);
    virtual ~IMessage();

    /**
     * <p> Сериализует объект в поток. </p>
     * @brief serialize
     * @param os - Поток, в который осуществляется сериализация.
     * @return - Размер сериализованных данных.
     */
    virtual size_t serialize(std::ostream &os) const = 0;

    /**
     * <p> Десериализует объект из потока ввода. </p>
     * @brief deserialize
     * @param is - Поток ввода, из которого десериализуется объект.
     * @return - Размер десериализованных данных объекта.
     */
    virtual size_t deserialize(std::istream& is) = 0;

    /**
     * <p> Возвращает тип сообщения (MessageType). </p>
     * @brief get_msg_type
     * @return
     */
    virtual int get_msg_type() const = 0;

    /**
     * <p> Возвращает идентификатор сообщения. </p>
     * @brief get_id
     * @return - Идентификатор сообщения.
     */
    unsigned long long get_id();

    /**
     * <p> Устанавливает содержимое сообщения. </p>
     * @brief set_msg_content
     * @param - Умный указатель на содержимое сообщения.
     */
    void set_msg_content(const std::shared_ptr<IMessageContent>& msg_content_ptr);

    /**
     * <p> Возвращает содержимое сообщения. </p>
     * @brief get_msg_content
     * @return - Умный указатель на содержимое сообщения
     */
    const std::shared_ptr<IMessageContent> get_msg_content() const;

protected:
    unsigned long long id_;
    std::shared_ptr<IMessageContent> msg_content_ptr_;
};

#endif // IMESSAGE_H
