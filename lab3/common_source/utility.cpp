#include "utility.h"

#include <algorithm>
#include <iostream>

Utility::Utility()
{

}

size_t Utility::serialize(std::ostream& os, const std::string& value) {
    const auto pos = os.tellp();

    // Приводим к типу uint32_t, т.к. на разных машинах размер машинного слова отличается,
    // что может привести к проблемам.
    const auto len = static_cast<uint32_t>(value.size());

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
