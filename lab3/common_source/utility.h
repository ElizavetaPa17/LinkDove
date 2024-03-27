#ifndef UTILITY_H
#define UTILITY_H

#include <type_traits>
#include <iostream>
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
     * <p> Сериализует фундаментальное значение в поток. </p>
     * @brief serialize_fundamental
     * @param os - Поток, в который осуществляется сериализация
     * @param value - Целочисленное значение, которое сериализуется
     * @return - Размер сериализованных данных
     */
    template <typename T>
    static typename std::enable_if<std::is_fundamental<T>::value, size_t>::type
    serialize_fundamental(std::ostream &os, T value);

    /**
     * <p> Десериализует фундаментальное значение из потока. </p>
     * @brief deserialize
     * @param is - Поток, из которого осуществляется десериализация.
     * @return - Пара - <размер десериализованных данных, десериализованное целочисленное значение>.
     */
    template<typename T>
    static typename std::enable_if<std::is_fundamental<T>::value, std::pair<size_t, T>>::type
    deserialize_fundamental(std::istream &is);
};

template <typename T>
typename std::enable_if<std::is_fundamental<T>::value, size_t>::type
Utility::serialize_fundamental(std::ostream &os, T value) {
    const auto pos = os.tellp();

    // Приводим к типу uint32_t, т.к. на разных машинах размер машинного слова отличается,
    // что может привести к проблемам.
    const auto len = static_cast<uint32_t>(sizeof(value));

    os.write(reinterpret_cast<const char*>(&len), sizeof(len));
    os.write(reinterpret_cast<const char*>(&value), sizeof(value));

    return static_cast<size_t>(os.tellp() - pos);
}

template<typename T>
typename std::enable_if<std::is_fundamental<T>::value, std::pair<size_t, T>>::type
Utility::deserialize_fundamental(std::istream &is) {
    T value{};
    uint32_t len = 0; // Размер сериализованной строки был записан в формате uint32_t

    is.read(reinterpret_cast<char*>(&len), sizeof(len));
    is.read(reinterpret_cast<char*>(&value), len);

    return std::make_pair(static_cast<size_t>(len), value);
}

#endif // UTILITY_H
