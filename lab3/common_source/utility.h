#ifndef UTILITY_H
#define UTILITY_H

#include <type_traits>
#include <iostream>
#include <vector>
#include <utility>
#include <memory>
#include <QLayout>

#include "complaint.h"
#include "imessage.h"
#include "StatusInfo.h"

/**
 * @brief The UtilitySerializator class
 * Отвечает за сериализацию и десериализацию объектов
 */
class UtilitySerializator final
{
public:
    UtilitySerializator() = delete;

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
     * <p> Сериализует вектор жалоб в поток. </p>
     * @brief serialize
     * @param os - Поток, в который осуществляется сериализация.
     * @param value - Сериализуемый вектор жалоб.
     * @return - Размер сериализованных данных.
     */
    static size_t serialize(std::ostream &os, const std::vector<Complaint>& value);

    /**
     * <p> Десериализует вектор жалоб из потока. </p>
     * @brief deserialize_compl_vec
     * @param is - Поток, из которого осуществляется десериализация.
     * @return - Пара - <размер десериализованных данных, десериализованный вектор жалоб>.
     */
    static std::pair<size_t, std::vector<Complaint>> deserialize_compl_vec(std::istream& is);

    /**
     * <p> Сериализует сообщение в поток. </p>
     * @brief serialize
     * @param os - Поток, в который осуществляется сериализация.
     * @param msg - Сериализуемое сообщение
     * @return - Размер сериализованных данных.
     */
    static size_t serialize(std::ostream &os, const IMessage& msg);

    /**
     * <p> Десериализует сообщение из потока. </p>
     * @brief deserialize_msg
     * @param is - Поток, из которого осуществляется десериализация
     * @return - Пара - <размер десериализованных данных, умный указатель на десериализованное сообщение>
     */
    static std::pair<size_t, std::shared_ptr<IMessage>> deserialize_msg(std::istream& is);

    /**
     * <p> Сериализует вектор сообщений в поток. </p>
     * @brief serialize
     * @param os - Поток, в который осуществляется сериализация.
     * @param value - Сериализуемый вектор сообщений.
     * @return - Размер сериализованных данных.
     */
    static size_t serialize(std::ostream &os, const std::vector<std::shared_ptr<IMessage>> &value);

    /**
     * <p> Десериализует вектор сообщений из потока. </p>
     * @brief deserialize_msg_vec
     * @param is - Поток, из которого осуществляется десериализация
     * @return - Пара - <размер десериализованных данных, вектор умных указателей на десериализованное сообщение>
     */
    static std::pair<size_t, std::vector<std::shared_ptr<IMessage>>> deserialize_msg_vec(std::istream& is);

    /**
     * <p> Сериализует вектор из структур с информацией о пользователях. </p>
     * @brief serialize
     * @param os - Поток, в который осуществляется сериализация.
     * @param value - Сериализуемый вектор структур.
     * @return - Размер сериализованных данных.
     */
    static size_t serialize(std::ostream &os, const std::vector<StatusInfo> &value);

    /**
     * <p> Десериализует вектор структур с информацией о пользователях из потока. </p>
     * @brief deserialize_st_info_vec
     * @param is - Поток, из которого осуществляется десериализация
     * @return - Пара - <размер десериализованных данных, вектор структур с информацией о пользователях.
     */
    static std::pair<size_t, std::vector<StatusInfo>> deserialize_st_info_vec(std::istream& is);

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
UtilitySerializator::serialize_fundamental(std::ostream &os, T value) {
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
UtilitySerializator::deserialize_fundamental(std::istream &is) {
    T value{};
    uint32_t len = 0; // Размер сериализованной строки был записан в формате uint32_t

    is.read(reinterpret_cast<char*>(&len), sizeof(len));
    is.read(reinterpret_cast<char*>(&value), len);

    return std::make_pair(static_cast<size_t>(len), value);
}

/**
 * @brief The QtUtility class
 * Утилита для части кода, написанной при помощи Qt.
 */
class QtUtility final {
public:
    /**
     * <p> Удаляет все виджеты и объекты из компоновщика. </p>
     * @brief clean_layout
     * @param layout - Очищаемый компоновщик.
     */
    static void clean_layout(QLayout *layout);

    /**
     * <p> Рекурсивно удаляет виджеты и компоновщики, которые содержатся в переданном компоновщике. </p>
     * @brief clean_complex_layout
     * @param layout - Очищаемый компоновщик.
     */
    static void clean_complex_layout(QLayout *layout);
};

#endif // UTILITY_H
