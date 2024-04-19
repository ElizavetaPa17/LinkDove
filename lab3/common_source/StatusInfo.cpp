#include "StatusInfo.h"

#include "constants.h"
#include "utility.h"

#include <QString>
#include <iostream>

size_t StatusInfo::serialize(std::ostream &os) const {
    size_t size = 0;

    size += UtilitySerializator::serialize_fundamental<unsigned long long>(os, id_);
    size += UtilitySerializator::serialize(os, username_);
    size += UtilitySerializator::serialize(os, email_);
    size += UtilitySerializator::serialize(os, birthday_.toString(BIRTHAY_FORMAT).toStdString());
    size += UtilitySerializator::serialize(os, text_status_);
    size += UtilitySerializator::serialize(os, image_bytes_);
    size == UtilitySerializator::serialize_fundamental<bool>(os, is_banned_);

    return size;
}

size_t StatusInfo::deserialize(std::istream &is) {
    size_t size = 0;

    std::pair<size_t, unsigned long long> temp_ullong_pair;
    temp_ullong_pair = UtilitySerializator::deserialize_fundamental<unsigned long long>(is);
    size += temp_ullong_pair.first;
    id_ = temp_ullong_pair.second;

    std::pair<size_t, std::string> temp_str_pair;
    temp_str_pair = UtilitySerializator::deserialize_string(is);
    size += temp_str_pair.first;
    username_ = temp_str_pair.second;

    temp_str_pair = UtilitySerializator::deserialize_string(is);
    size += temp_str_pair.first;
    email_ = temp_str_pair.second;

    temp_str_pair = UtilitySerializator::deserialize_string(is);
    size += temp_str_pair.first;
    birthday_ = QDate::fromString(QString(temp_str_pair.second.c_str()), BIRTHAY_FORMAT);

    temp_str_pair = UtilitySerializator::deserialize_string(is);
    size += temp_str_pair.first;
    text_status_ = temp_str_pair.second;

    std::pair<size_t, std::vector<char>> temp_vec_pair;
    temp_vec_pair = UtilitySerializator::deserialize_char_vec(is);
    size += temp_vec_pair.first;
    image_bytes_ = temp_vec_pair.second;

    std::pair<size_t, bool> temp_bool_pair;
    temp_bool_pair = UtilitySerializator::deserialize_fundamental<bool>(is);
    size += temp_bool_pair.first;
    is_banned_ = temp_bool_pair.second;

    return size;
}
