#ifndef CHANNELINFO_H
#define CHANNELINFO_H

#include <string>

#include "constants.h"

struct ChannelInfo {
public:
    unsigned long long id_ = CREATED_ENTITY_ID;
    unsigned long long owner_id_ = CREATED_ENTITY_ID;
    std::string name_;
    bool is_banned_ = false;

    /**
     * <p> Сериализует текущее состояние объекта. </p>
     * @brief serialize
     * @param os - Поток, в который сериализуются данные.
     * @return - Размер сериализованных данных объекта.
     */
    size_t serialize(std::ostream& os) const;

    /**
     * <p> Десериализует объект из потока ввода. </p>
     * @brief deserialize
     * @param is - Поток ввода, из которого десериализуется объект.
     * @return - Размер десериализованных данных объекта.
     */
    size_t deserialize(std::istream& is);
};

#endif // CHANNELINFO_H
