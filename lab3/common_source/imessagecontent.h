#ifndef IMESSAGECONTENT_H
#define IMESSAGECONTENT_H

#include <cstddef>
#include <iostream>

class IMessageContent
{
public:
    IMessageContent();
    ~IMessageContent();

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
     * <p> Возвращает указатель на данные, хранящиеся в сообщении. </p>
     * @brief get_raw_data
     * @return - Указатель на данные.
     */
    virtual const char* get_raw_data() const = 0;

    /**
     * <p> Возвращает тип содержимого сообщения. </p>
     * @brief get_msg_type
     * @return - Тип сообщения (enum MessageContent Type).
     */
    virtual int get_msg_content_type() const = 0;
};

#endif // IMESSAGECONTENT_H
