#ifndef UTILITY_H
#define UTILITY_H

#include <ostream>

class Utility final
{
public:
    Utility();

    /**
     * <p> Сериализует строку в поток. </p>
     * @brief serialize
     * @param os - Поток, в который осуществляется сериализация.
     * @param value - Сериализуемая строка.
     * @return - Размер сериализованных данных.
     */
    static size_t serialize(std::ostream& os, const std::string& value);

    /**
     * <p> Сериализует вектор символов в поток. </p>
     * @brief serialize
     * @param os - Поток, в который осуществляется сериализация.
     * @param value - Сериализуемый вектор символов.
     * @return - Размер сериализованных данных.
     */
    static size_t serialize(std::ostream &os, const std::vector<char>& value);
};

#endif // UTILITY_H
