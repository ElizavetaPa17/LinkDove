#include "utility.h"

#include <algorithm>
#include <iostream>

#include <QWidget>
#include <QDir>
#include <QFileInfo>
#include <QFile>

#include "individualmessage.h"
#include "textmessagecontent.h"
#include "groupmessage.h"

size_t UtilitySerializator::serialize(std::ostream& os, const std::string& value) {
    const auto pos = os.tellp();

    // Приводим к типу uint32_t, т.к. на разных машинах размер машинного слова отличается,
    // что может привести к проблемам.
    const uint32_t len = static_cast<uint32_t>(value.size());

    // Сериализуем сначала размер строки, а потом саму строку.
    os.write(reinterpret_cast<const char*>(&len), sizeof(len));
    if (len > 0) {
        os.write(value.data(), value.size());
    }

    return static_cast<size_t>(os.tellp() - pos);
}

std::pair<size_t, std::string> UtilitySerializator::deserialize_string(std::istream& is) {
    std::string str;
    uint32_t len = 0; // Размер сериализованной строки был записан в формате uint32_t

    is.read(reinterpret_cast<char*>(&len), sizeof(len));

    if (len > 0) {
        str.resize(len);
        is.read(str.data(), len);
    }

    return std::make_pair(static_cast<size_t>(len), str);
}

size_t UtilitySerializator::serialize(std::ostream &os, const std::vector<std::string> &vec) {
    const auto pos = os.tellp();

    // Приводим к типу uint32_t, т.к. на разных машинах размер машинного слова отличается,
    // что может привести к проблемам.
    const auto len = static_cast<uint32_t>(vec.size());

    // Сериализуем сначала размер вектора, а потом сам вектор.
    os.write(reinterpret_cast<const char*>(&len), sizeof(len));
    if (len > 0) {
        for (int i = 0; i < len; ++i) {
            UtilitySerializator::serialize(os, vec[i]);
        }
    }

    return static_cast<size_t>(os.tellp() - pos);
}

std::pair<size_t, std::vector<std::string>> UtilitySerializator::deserialize_vec_string(std::istream& is) {
    std::vector<std::string> vec;
    uint32_t len = 0; // Размер сериализованной строки был записан в формате uint32_t

    is.read(reinterpret_cast<char*>(&len), sizeof(len));
    if (len > 0) {
        vec.resize(len);

        for (int i = 0; i < len; ++i) {
            vec[i] = UtilitySerializator::deserialize_string(is).second;
        }
    }

    return std::make_pair(static_cast<size_t>(len), vec);
}

size_t UtilitySerializator::serialize(std::ostream &os, const std::vector<Complaint>& value) {
    const auto pos = os.tellp();

    // Приводим к типу uint32_t, т.к. на разных машинах размер машинного слова отличается,
    // что может привести к проблемам.
    const auto len = static_cast<uint32_t>(value.size());

    // Сериализуем сначала размер вектора, а потом сам вектор.
    os.write(reinterpret_cast<const char*>(&len), sizeof(len));
    if (len > 0) {
        for (int i = 0; i < len; ++i) {
            value[i].serialize(os);
        }
    }

    return static_cast<size_t>(os.tellp() - pos);
}

std::pair<size_t, std::vector<Complaint>> UtilitySerializator::deserialize_compl_vec(std::istream& is) {
    std::vector<Complaint> vec;
    uint32_t len = 0; // Размер сериализованной строки был записан в формате uint32_t

    is.read(reinterpret_cast<char*>(&len), sizeof(len));
    if (len > 0) {
        vec.resize(len);

        for (int i = 0; i < len; ++i) {
            vec[i].deserialize(is);
        }
    }

    return std::make_pair(static_cast<size_t>(len), vec);
}

size_t UtilitySerializator::serialize(std::ostream &os, const std::vector<Notification>& value) {
    const auto pos = os.tellp();

    // Приводим к типу uint32_t, т.к. на разных машинах размер машинного слова отличается,
    // что может привести к проблемам.
    const auto len = static_cast<uint32_t>(value.size());

    // Сериализуем сначала размер вектора, а потом сам вектор.
    os.write(reinterpret_cast<const char*>(&len), sizeof(len));
    if (len > 0) {
        for (int i = 0; i < len; ++i) {
            value[i].serialize(os);
        }
    }

    return static_cast<size_t>(os.tellp() - pos);
}

std::pair<size_t, std::vector<Notification>> UtilitySerializator::deserialize_not_vec(std::istream& is) {
    std::vector<Notification> vec;
    uint32_t len = 0; // Размер сериализованной строки был записан в формате uint32_t

    is.read(reinterpret_cast<char*>(&len), sizeof(len));
    if (len > 0) {
        vec.resize(len);

        for (int i = 0; i < len; ++i) {
            vec[i].deserialize(is);
        }
    }

    return std::make_pair(static_cast<size_t>(len), vec);
}

size_t UtilitySerializator::serialize(std::ostream &os, const IMessage& msg) {
    const auto pos = os.tellp();

    // Сериализуем тип сообщения.
    auto type = static_cast<uint32_t>(msg.get_msg_type());
    os.write(reinterpret_cast<const char*>(&type), sizeof(type));

    // Сериализуем тип содержимого сообщения
    type = static_cast<uint32_t>(msg.get_msg_content()->get_msg_content_type());
    os.write(reinterpret_cast<const char*>(&type), sizeof(type));

    msg.serialize(os);

    return static_cast<size_t>(os.tellp() - pos);
}

std::pair<size_t, std::shared_ptr<IMessage>> UtilitySerializator::deserialize_msg(std::istream& is) {
    std::shared_ptr<IMessage> msg_ptr;
    uint32_t type = 0;
    size_t size = 0;

    // Десериализуем тип сообщения
    is.read(reinterpret_cast<char*>(&type), sizeof(type));
    switch (type) {
        case INDIVIDUAL_MSG_TYPE: {
            msg_ptr = std::make_shared<IndividualMessage>();
            break;
        }
        case CHANNEL_MSG_TYPE: {
            msg_ptr = std::make_shared<ChannelMessage>();
            break;
        }
        case GROUP_MSG_TYPE: {
            msg_ptr = std::make_shared<GroupMessage>();
        }
    }

    // Десериализуем тип содержимого сообщения
    is.read(reinterpret_cast<char*>(&type), sizeof(type));
    switch(type) {
        case TEXT_MSG_TYPE: {
            msg_ptr->set_msg_content(std::make_shared<TextMessageContent>());
            break;
        }

        case IMAGE_MSG_TYPE: {
            msg_ptr->set_msg_content(std::make_shared<ImageMessageContent>());
            break;
        }
    }

    size += msg_ptr->deserialize(is) + 2*sizeof(type);
    return std::pair<size_t, std::shared_ptr<IMessage>>(size, msg_ptr);
}

size_t UtilitySerializator::serialize(std::ostream &os, const std::vector<std::shared_ptr<IMessage>> &value) {
    const auto pos = os.tellp();

    // Приводим к типу uint32_t, т.к. на разных машинах размер машинного слова отличается,
    // что может привести к проблемам.
    const auto len = static_cast<uint32_t>(value.size());

    // Сериализуем сначала размер вектора, а потом сам вектор.
    os.write(reinterpret_cast<const char*>(&len), sizeof(len));
    for (int i = 0; i < len; ++i) {
        UtilitySerializator::serialize(os, *value[i]);
    }

    return static_cast<size_t>(os.tellp() - pos);
}

std::pair<size_t, std::vector<std::shared_ptr<IMessage>>> UtilitySerializator::deserialize_msg_vec(std::istream& is) {
    std::vector<std::shared_ptr<IMessage>> vec;
    uint32_t len = 0; // Размер сериализованной строки был записан в формате uint32_t

    is.read(reinterpret_cast<char*>(&len), sizeof(len));
    if (len > 0) {
        vec.resize(len);

        std::cerr << len << '\n';

        for (int i = 0; i < len; ++i) {
            vec[i] = UtilitySerializator::deserialize_msg(is).second;
        }
    }

    return std::make_pair(static_cast<size_t>(len), vec);
}

size_t UtilitySerializator::serialize(std::ostream &os, const std::vector<StatusInfo> &value) {
    const auto pos = os.tellp();

    // Приводим к типу uint32_t, т.к. на разных машинах размер машинного слова отличается,
    // что может привести к проблемам.
    const auto len = static_cast<uint32_t>(value.size());

    // Сериализуем сначала размер вектора, а потом сам вектор.
    os.write(reinterpret_cast<const char*>(&len), sizeof(len));
    for (int i = 0; i < len; ++i) {
        value[i].serialize(os);
    }

    return static_cast<size_t>(os.tellp() - pos);
}

std::pair<size_t, std::vector<StatusInfo>> UtilitySerializator::deserialize_st_info_vec(std::istream& is) {
    std::vector<StatusInfo> vec;
    uint32_t len = 0; // Размер сериализованной строки был записан в формате uint32_t

    is.read(reinterpret_cast<char*>(&len), sizeof(len));
    if (len > 0) {
        vec.resize(len);

        for (int i = 0; i < len; ++i) {
            vec[i].deserialize(is);
        }
    }

    return std::make_pair(static_cast<size_t>(len), vec);
}

size_t UtilitySerializator::serialize(std::ostream &os, const std::vector<ChannelInfo> &value) {
    const auto pos = os.tellp();

    // Приводим к типу uint32_t, т.к. на разных машинах размер машинного слова отличается,
    // что может привести к проблемам.
    const auto len = static_cast<uint32_t>(value.size());

    // Сериализуем сначала размер вектора, а потом сам вектор.
    os.write(reinterpret_cast<const char*>(&len), sizeof(len));
    for (int i = 0; i < len; ++i) {
        value[i].serialize(os);
    }

    return static_cast<size_t>(os.tellp() - pos);
}

std::pair<size_t, std::vector<ChannelInfo>> UtilitySerializator::deserialize_ch_info_vec(std::istream& is) {
    std::vector<ChannelInfo> vec;
    uint32_t len = 0; // Размер сериализованной строки был записан в формате uint32_t

    is.read(reinterpret_cast<char*>(&len), sizeof(len));
    if (len > 0) {
        vec.resize(len);

        for (int i = 0; i < len; ++i) {
            vec[i].deserialize(is);
        }
    }

    return std::make_pair(static_cast<size_t>(len), vec);
}

size_t UtilitySerializator::serialize(std::ostream &os, const std::vector<ChatInfo> &value) {
    const auto pos = os.tellp();

    // Приводим к типу uint32_t, т.к. на разных машинах размер машинного слова отличается,
    // что может привести к проблемам.
    const auto len = static_cast<uint32_t>(value.size());

    // Сериализуем сначала размер вектора, а потом сам вектор.
    os.write(reinterpret_cast<const char*>(&len), sizeof(len));
    for (int i = 0; i < len; ++i) {
        value[i].serialize(os);
    }

    return static_cast<size_t>(os.tellp() - pos);
}

std::pair<size_t, std::vector<ChatInfo>> UtilitySerializator::deserialize_chat_info_vec(std::istream& is) {
    std::vector<ChatInfo> vec;
    uint32_t len = 0; // Размер сериализованной строки был записан в формате uint32_t

    is.read(reinterpret_cast<char*>(&len), sizeof(len));
    if (len > 0) {
        vec.resize(len);

        for (int i = 0; i < len; ++i) {
            vec[i].deserialize(is);
        }
    }

    return std::make_pair(static_cast<size_t>(len), vec);
}

void QtUtility::clean_layout(QLayout *layout) {
    QLayoutItem *widget_item = nullptr;

    while ((widget_item = layout->takeAt(0)) != nullptr) {
        if (widget_item->widget()) {
            widget_item->widget()->deleteLater();
        }

        delete widget_item;
    }
}

void QtUtility::clean_complex_layout(QLayout *layout) {
    if (layout == NULL) {
            return;
    }

    for(QLayoutItem* item{}; item = layout->takeAt(0);) {
        QtUtility::clean_complex_layout(item->layout());
        if (item->widget()) {
            delete item->widget();
        }

        delete item;
    }
}

QString QtUtility::get_random_string(size_t length) {
    static const QString possible_characters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");

   QString random_string;
   for(int i=0; i < length; ++i)
   {
       int index = random() % possible_characters.length();
       QChar nextChar = possible_characters.at(index);
       random_string.append(nextChar);
   }

   return random_string;
}

std::shared_ptr<IndividualMessage> MessageUtility::create_individual_text_message(unsigned long long sender_id,
                                                                               unsigned long long receiver_id,
                                                                               const std::string& text)
{
    std::shared_ptr<IndividualMessage> ind_message = std::make_shared<IndividualMessage>();
    ind_message->set_msg_edges(sender_id, receiver_id);

    std::shared_ptr<TextMessageContent> text_msg_content_ptr = std::make_shared<TextMessageContent>();
    text_msg_content_ptr->set_text(text);
    ind_message->set_msg_content(text_msg_content_ptr);

    return ind_message;
}

std::shared_ptr<IndividualMessage> MessageUtility::create_individual_image_message(unsigned long long sender_id,
                                                                                   unsigned long long receiver_id,
                                                                                   const std::string& image_path)
{
    std::shared_ptr<IndividualMessage> ind_message = std::make_shared<IndividualMessage>();
    ind_message->set_msg_edges(sender_id, receiver_id);

    std::shared_ptr<ImageMessageContent> image_msg_content_ptr = std::make_shared<ImageMessageContent>();
    image_msg_content_ptr->set_image_path(image_path);
    ind_message->set_msg_content(image_msg_content_ptr);

    return ind_message;
}

std::shared_ptr<ChannelMessage> MessageUtility::create_channel_text_message(unsigned long long channel_id,
                                                                                   const std::string &text)
{
    std::shared_ptr<ChannelMessage> ind_message = std::make_shared<ChannelMessage>();
    ind_message->set_channel_id(channel_id);

    std::shared_ptr<TextMessageContent> text_msg_content_ptr = std::make_shared<TextMessageContent>();
    text_msg_content_ptr->set_text(text);
    ind_message->set_msg_content(text_msg_content_ptr);

    return ind_message;
}

std::shared_ptr<ChannelMessage> MessageUtility::create_channel_image_message(unsigned long long channel_id,
                                                                             const std::string &image_path)
{
    std::shared_ptr<ChannelMessage> ind_message = std::make_shared<ChannelMessage>();
    ind_message->set_channel_id(channel_id);

    std::shared_ptr<ImageMessageContent> text_msg_content_ptr = std::make_shared<ImageMessageContent>();
    text_msg_content_ptr->set_image_path(image_path);
    ind_message->set_msg_content(text_msg_content_ptr);

    return ind_message;
}

std::shared_ptr<GroupMessage> MessageUtility::create_group_text_message(unsigned long long group_id,
                                                                        unsigned long long owner_id,
                                                                        const std::string &text)
{
    std::shared_ptr<GroupMessage> message = std::make_shared<GroupMessage>();
    message->set_group_id(group_id);
    message->set_owner_id(owner_id);

    std::shared_ptr<TextMessageContent> text_msg_content_ptr = std::make_shared<TextMessageContent>();
    text_msg_content_ptr->set_text(text);
    message->set_msg_content(text_msg_content_ptr);

    return message;
}

std::shared_ptr<GroupMessage> MessageUtility::create_group_image_message(unsigned long long group_id,
                                                           unsigned long long owner_id,
                                                           const std::string &image_path)
{
    std::shared_ptr<GroupMessage> message = std::make_shared<GroupMessage>();
    message->set_group_id(group_id);
    message->set_owner_id(owner_id);

    std::shared_ptr<ImageMessageContent> text_msg_content_ptr = std::make_shared<ImageMessageContent>();
    text_msg_content_ptr->set_image_path(image_path);
    message->set_msg_content(text_msg_content_ptr);

    return message;
}

std::string MessageUtility::copy_image_to_ind_folder(const QString &image_path) {
    QString new_image_path = QString(MEDIA_IND_IMAGE_PATH) + QFileInfo(image_path).fileName();
    if (!QFile::exists(new_image_path)) {
            if (!QFile::copy(image_path, new_image_path)) {
                std::runtime_error("Unable to copy image into ind_folder\n");
            }
    }

    return new_image_path.toStdString();
}

std::string MessageUtility::copy_image_to_channel_folder(const QString& image_path) {
    QString new_image_path = QString(MEDIA_CHANNEL_IMAGE_PATH) + QFileInfo(image_path).fileName();
    if (!QFile::exists(new_image_path)) {
            if (!QFile::copy(image_path, new_image_path)) {
                std::runtime_error("Unable to copy image into channel_folder\n");
            }
    }

    return new_image_path.toStdString();
}

std::string MessageUtility::copy_image_to_chat_folder(const QString& image_path) {
    QString new_image_path = QString(MEDIA_CHAT_IMAGE_PATH) + QFileInfo(image_path).fileName();
    if (!QFile::exists(new_image_path)) {
            if (!QFile::copy(image_path, new_image_path)) {
                std::runtime_error("Unable to copy image into chat_folder\n");
            }
    }

    return new_image_path.toStdString();
}

std::string MessageUtility::copy_image_to_avatars_folder(const QString& image_path) {
    QString new_image_path = QString(MEDIA_AVATARS_IMAGE_PATH) + QFileInfo(image_path).fileName();
    if (!QFile::exists(new_image_path)) {
            if (!QFile::copy(image_path, new_image_path)) {
                std::runtime_error("Unable to copy image into chat_folder\n");
            }
    }

    return new_image_path.toStdString();
}
