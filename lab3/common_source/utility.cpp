#include "utility.h"

#include <algorithm>
#include <iostream>

#include "individualmessage.h"
#include "textmessagecontent.h"

size_t Utility::serialize(std::ostream& os, const std::string& value) {
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

std::pair<size_t, std::string> Utility::deserialize_string(std::istream& is) {
    std::string str;
    uint32_t len = 0; // Размер сериализованной строки был записан в формате uint32_t

    is.read(reinterpret_cast<char*>(&len), sizeof(len));

    if (len > 0) {
        str.resize(len);
        is.read(str.data(), len);
    }

    return std::make_pair(static_cast<size_t>(len), str);
}

size_t Utility::serialize(std::ostream &os, const std::vector<char>& value) {
    const auto pos = os.tellp();

    // Приводим к типу uint32_t, т.к. на разных машинах размер машинного слова отличается,
    // что может привести к проблемам.
    const auto len = static_cast<uint32_t>(value.size());

    // Сериализуем сначала размер вектора, а потом сам вектор.
    os.write(reinterpret_cast<const char*>(&len), sizeof(len));
    if (len > 0) {
        os.write(value.data(), value.size());
    }

    return static_cast<size_t>(os.tellp() - pos);
}

std::pair<size_t, std::vector<char>> Utility::deserialize_char_vec(std::istream& is) {
    std::vector<char> vec;
    uint32_t len = 0; // Размер сериализованной строки был записан в формате uint32_t

    is.read(reinterpret_cast<char*>(&len), sizeof(len));
    if (len > 0) {
        vec.resize(len);
        is.read(vec.data(), len);
    }

    return std::make_pair(static_cast<size_t>(len), vec);
}

size_t Utility::serialize(std::ostream &os, const std::vector<Complaint>& value) {
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

std::pair<size_t, std::vector<Complaint>> Utility::deserialize_compl_vec(std::istream& is) {
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

size_t Utility::serialize(std::ostream &os, const IMessage& msg) {
    const auto pos = os.tellp();

    std::cerr << msg.get_msg_type();
    // Сериализуем тип сообщения.
    auto type = static_cast<uint32_t>(msg.get_msg_type());
    os.write(reinterpret_cast<const char*>(&type), sizeof(type));

    // Сериализуем тип содержимого сообщения
    type = static_cast<uint32_t>(msg.get_msg_content()->get_msg_content_type());
    os.write(reinterpret_cast<const char*>(&type), sizeof(type));

    msg.serialize(os);

    return static_cast<size_t>(os.tellp() - pos);
}

std::pair<size_t, std::shared_ptr<IMessage>> Utility::deserialize_msg(std::istream& is) {
    std::shared_ptr<IMessage> msg_ptr;
    uint32_t type = 0;
    size_t size = 0;

    // Десериализуем тип сообщения
    is.read(reinterpret_cast<char*>(&type), sizeof(type));
    switch (type) {
        case INDIVIDUAL_MSG_TYPE: {
            msg_ptr = std::make_shared<IndividualMessage>(CREATED_MSG_ID);
            break;
        }
        case BROAD_MSG_TYPE: {
            break;
        }
    }

    // Десериализуем тип содержимого сообщения
    is.read(reinterpret_cast<char*>(&type), sizeof(type));
    switch(type) {
        case TEXT_MSG_TYPE: {
            msg_ptr->set_msg_content(std::make_shared<TextMessageContent>());
            break;
        }
    }

    size += msg_ptr->deserialize(is) + 2*sizeof(type);
    return std::pair<size_t, std::shared_ptr<IMessage>>(size, msg_ptr);
}
