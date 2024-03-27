#ifndef STATUSINFO_H
#define STATUSINFO_H

#include <string>
#include <vector>
#include <QDate>

struct StatusInfo final {
    unsigned long long id_ = 0;
    std::string username_;
    std::string email_;
    QDate       birthday_;
    std::string text_status_;
    std::vector<char> image_bytes_;
    bool is_banned_ = false;

    /**
     * <p> Сериализует текущее состояние объекта. </p>
     * @brief serialize
     * @param os - Поток, в который сериализуются данные.
     * @return - Размер сериализованных данных объекта.
     */
    size_t serialize(std::ostream& os);

    /**
     * <p> Десериализует объект из потока ввода. </p>
     * @brief deserialize
     * @param is - Поток ввода, из которого десериализуется объект.
     * @return - Размер десериализованных данных объекта.
     */
    size_t deserialize(std::istream& is);
};

#endif // STATUSINFO_H
