#include "utility.h"

#include <algorithm>

Utility::Utility()
{

}

size_t Utility::serialize(std::ostream& os, const std::string& value) {
    const auto pos = os.tellp();

    // Приводим к типу uint32_t, т.к. на разных машинах размер машинного слова отличается,
    // что может привести к проблемам.
    const auto len = static_cast<uint32_t>(value.size());

    // Сериализуем сначала размер строки, а потом саму строку.
    os.write(reinterpret_cast<char*>(&len), sizeof(len));
    if (len > 0) {
        os.write(value.data(), value.size());
    }

    return static_cast<size_t>(os.tellp() - pos);
}

size_t Utility::serialize(std::ostream &os, const std::vector<char>& value) {
    const auto pos = os.tellp();

    // Приводим к типу uint32_t, т.к. на разных машинах размер машинного слова отличается,
    // что может привести к проблемам.
    const auto len = static_cast<uint32_t>(value.size());

    // Сериализуем сначала размер вектора, а потом сам вектор.
    os.write(reinterpret_cast<char*>(&len), sizeof(len));
    if (len > 0) {
        os.write(value.data(), value.size());
    }

    return static_cast<size_t>(os.tellp() - pos);
}

