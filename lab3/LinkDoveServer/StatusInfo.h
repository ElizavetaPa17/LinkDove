#ifndef STATUSINFO_H
#define STATUSINFO_H

#include <string>
#include <vector>
#include <QDate>

struct StatusInfo final {
    std::string username_;
    std::string email_;
    QDate       birthday_;
    std::string text_status_;
    std::vector image_bytes_;

    /**
     * <p> Сериализует текущее состояние объекта. </p>
     * @brief serialize
     * @param os - Поток, в который сериализуются данные.
     * @return - Размер сериализованных данных объекта.
     */
    size_t serialize(std::ostream& os);
}

#endif // STATUSINFO_H
