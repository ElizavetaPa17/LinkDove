#ifndef SHORTSTATUSINFO_H
#define SHORTSTATUSINFO_H

#include <string>

class ShortStatusInfo final {
    unsigned long long id_ = 0;
    std::string username_;
public:

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

#endif // SHORTSTATUSINFO_H
