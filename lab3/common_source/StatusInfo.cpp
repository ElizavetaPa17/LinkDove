#include "StatusInfo.h"

#include "utility.h"
#include <QString>
#include <iostream>

#define BIRTHAY_FORMAT "yyyy-MM-dd"

size_t StatusInfo::serialize(std::ostream &os) {
    size_t size = 0;

    size += Utility::serialize(os, username_);
    size += Utility::serialize(os, email_);
    size += Utility::serialize(os, birthday_.toString(BIRTHAY_FORMAT).toStdString());
    size += Utility::serialize(os, text_status_);
    size += Utility::serialize(os, image_bytes_);

    return size;
}

size_t StatusInfo::deserialize(std::istream &is) {
    size_t size = 0;
    std::pair<size_t, std::string> temp_str_pair;

    temp_str_pair = Utility::deserialize_string(is);
    size += temp_str_pair.first;
    username_ = temp_str_pair.second;

    temp_str_pair = Utility::deserialize_string(is);
    size += temp_str_pair.first;
    email_ = temp_str_pair.second;

    temp_str_pair = Utility::deserialize_string(is);
    size += temp_str_pair.first;
    birthday_ = QDate::fromString(QString(temp_str_pair.second.c_str()), BIRTHAY_FORMAT);

    temp_str_pair = Utility::deserialize_string(is);
    size += temp_str_pair.first;
    text_status_ = temp_str_pair.second;

    std::pair<size_t, std::vector<char>> temp_vec_pair;
    temp_vec_pair = Utility::deserialize_char_vec(is);
    size += temp_vec_pair.first;
    image_bytes_ = temp_vec_pair.second;

    return size;
}
