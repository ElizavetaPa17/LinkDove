#ifndef USERINFO_H
#define USERINFO_H

#include "StatusInfo.h"

struct UserInfo final {
    StatusInfo status_info_;
    std::string password_;

    /**
     * <p> Сериализует текущее состояние объекта. </p>
     * @brief serialize
     * @param os - Поток, в который сериализуются данные.
     * @return - Размер сериализованных данных объекта.
     */
    size_t serialize(std::ostream& os);
};

#endif // USERINFO_H
