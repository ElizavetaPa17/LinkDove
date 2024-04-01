#ifndef COMPLAINT_H
#define COMPLAINT_H

#include <string>

class Complaint final {
public:
    unsigned long long id_ = 0;
    unsigned long long sender_id_ = 0;
    std::string text_;

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

#endif // COMPLAINT_H
