#ifndef UTILITY_H
#define UTILITY_H

#include <ostream>
#include <vector>
#include <utility>

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
     * <p> Десериализует строку из потока. </p>
     * @brief deserialize_string
     * @param is - Поток, из которого осуществляется десериализация.
     * @return - Пара - <размер десериализованных данных, десериализованная строка>.
     */
    static std::pair<size_t, std::string> deserialize_string(std::istream& is);

    /**
     * <p> Сериализует вектор символов в поток. </p>
     * @brief serialize
     * @param os - Поток, в который осуществляется сериализация.
     * @param value - Сериализуемый вектор символов.
     * @return - Размер сериализованных данных.
     */
    static size_t serialize(std::ostream &os, const std::vector<char>& value);

    /**
     * <p> Десериализует вектор символов из потока. </p>
     * @brief deserialize_char_vec
     * @param is - Поток, из которого осуществляется десериализация.
     * @return - Пара - <размер десериализованных данных, десериализованный вектор символов>.
     */
    static std::pair<size_t, std::vector<char>> deserialize_char_vec(std::istream& is);

    /**
     * <p> Сериализует значение типа bool в поток. </p>
     * @brief serialize
     * @param os - Поток, в который осуществляется сериализация
     * @param value - Значение типа bool, которое сериализуется
     * @return - Размер сериализованных данных
     */
    static size_t serialize(std::ostream &os, bool value);

    /**
     * <p> Десериализует значение bool из потока. </p>
     * @brief deserialize
     * @param is - Поток, из которого осуществляется десериализация.
     * @return - Пара - <размер десериализованных данных, десериализованное значение типа bool>.
     */
    static std::pair<size_t, bool> deserialize_bool(std::istream& is);
};

#endif // UTILITY_H
