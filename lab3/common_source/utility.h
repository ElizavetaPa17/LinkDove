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
#include "channelinfo.h"
#include "chatinfo.h"

#include "individualmessage.h"
#include "channelmessage.h"
#include "groupmessage.h"
#include "textmessagecontent.h"
#include "imagemessagecontent.h"

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
     * @return - Пара - <размер десериализованных данных, вектор структур с информацией о пользователях>.
     */
    static std::pair<size_t, std::vector<StatusInfo>> deserialize_st_info_vec(std::istream& is);

    /**
     * <p> Сериализует вектор из структур с информацией о каналах. </p>
     * @brief serialize
     * @param os - Поток, в который осуществляется сериализация.
     * @param value - Сериализуемый вектор структур.
     * @return  - Размер сериализованных данных.
     */
    static size_t serialize(std::ostream &os, const std::vector<ChannelInfo> &value);

    /**
     * <p> Десериализует вектор структур с информацией о каналах из потока. </p>
     * @brief deserialize_ch_info_vec
     * @param is - Поток, из которого осуществляется десериализация
     * @return - Пара - <размер десериализованных данных, вектор структур с информацией о каналах>.
     */
    static std::pair<size_t, std::vector<ChannelInfo>> deserialize_ch_info_vec(std::istream& is);

    /**
     * <p> Сериализует вектор из структур с информацией о чатах. </p>
     * @brief serialize
     * @param os - Поток, в который осуществляется сериализация.
     * @param value - Сериализуемый вектор структур.
     * @return  - Размер сериализованных данных.
     */
    static size_t serialize(std::ostream &os, const std::vector<ChatInfo> &value);

    /**
     * <p> Десериализует вектор структур с информацией о чатах из потока. </p>
     * @brief deserialize_chat_info_vec
     * @param is - Поток, из которого осуществляется десериализация
     * @return - Пара - <размер десериализованных данных, вектор структур с информацией о каналах>.
     */
    static std::pair<size_t, std::vector<ChatInfo>> deserialize_chat_info_vec(std::istream& is);

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

class MessageUtility final {
public:
    /**
     * <p> Создает сообщение чата между двумя собеседниками с текстовым содержимым. </p>
     * @brief createIndividualTextMessage
     * @param sender_id - Идентификатор отправителя.
     * @param receiver_id -  Идентификатор получаталя.
     * @param text - Текст сообщения.
     * @return - Умный указатель на сообщение.
     */
    static std::shared_ptr<IndividualMessage> create_individual_text_message(unsigned long long sender_id,
                                                                          unsigned long long receiver_id,
                                                                          const std::string &text);

    /**
     * <p> Создает сообщение из чата между двумя собеседниками с содержанием картинки. </p>
     * @brief createIndividualImageMessage
     * @param sender_id - Идентификатор отправителя.
     * @param receiver_id - Идентификатор получателя.
     * @param image_path - Путь к картинке.
     * @return - Умный указатель на сообщение.
     */
    static std::shared_ptr<IndividualMessage> create_individual_image_message(unsigned long long sender_id,
                                                                           unsigned long long receiver_id,
                                                                           const std::string &image_path);

    /**
     * <p> Создает сообщение канала с текстовым содержимым. </p>
     * @brief create_channel_text_message
     * @param channel_id - Идентификатор канала.
     * @param text - Текст сообщения.
     * @return - Умный указатель на сообщение.
     */
    static std::shared_ptr<ChannelMessage> create_channel_text_message(unsigned long long channel_id,
                                                                       const std::string &text);

    /**
     * <p> Создает сообщение канала с содержанием картинки. </p>
     * @brief cerate_channel_image_message
     * @param channel_id - Идентификатор канала.
     * @param image_path - Путь к картинке.
     * @return  - Умный указатель на сообщение.
     */
    static std::shared_ptr<ChannelMessage> create_channel_image_message(unsigned long long channel_id,
                                                                        const std::string &image_path);

    /**
     * <p> Создает сообщение группы с текстовым содержимым. </p>
     * @brief create_group_text_message
     * @param group_id - Идентификатор группы.
     * @param group_id - Идентификатор владельца сообщения.
     * @param text - Текст сообщения.
     * @return - Умный указатель на сообщение.
     */
    static std::shared_ptr<GroupMessage> create_group_text_message(unsigned long long group_id,
                                                                     unsigned long long owner_id,
                                                                     const std::string &text);

    /**
     * <p> Создает сообщение группы с содержанием картинки. </p>
     * @brief cerate_group_image_message
     * @param group_id - Идентификатор группы.
     * @param group_id - Идентификатор владельца сообщения.
     * @param image_path - Путь к картинке.
     * @return  - Умный указатель на сообщение.
     */
    static std::shared_ptr<GroupMessage> create_group_image_message(unsigned long long group_id,
                                                                      unsigned long long owner_id,
                                                                      const std::string &image_path);

    /**
     * <p> Копирует изображение из места, куда указывает image_path, в MEDIA_IND_IMAGE_PATH. </p>
     * @brief move_image_to_db_folder
     * @param image_path - Старый путь к изображению.
     * @return - Путь к изображению в папке MEDIA_IND_IMAGE_PATH.
     */
    static std::string copy_image_to_ind_folder(const QString& image_path);

    /**
     * <p> Копирует изображение из места, куда указывает image_path, в MEDIA_CHANNEL_IMAGE_PATH. </p>
     * @brief copy_image_to_channel_folder
     * @param image_path - Старый путь к изображению.
     * @return - Путь к изображению в папке MEDIA_CHANNEL_IMAGE_PATH.
     */
    static std::string copy_image_to_channel_folder(const QString& image_path);

    /**
     * <p> Копирует изображение из места, куда указывает image_path, в MEDIA_CHAT_IMAGE_PATH. </p>
     * @brief copy_image_to_chat_folder
     * @param image_path - Старый путь к изображению.
     * @return - Путь к изображению в папке MEDIA_CHAT_IMAGE_PATH.
     */
    static std::string copy_image_to_chat_folder(const QString& image_path);
};

#endif // UTILITY_H
