#include "action.h"

#include <utility>
#include "utility.h"

size_t Action::serialize(std::ostream& os) const {
    size_t size = 0;

    size += UtilitySerializator::serialize(os, time_.toString(DATETIME_FORMAT).toStdString());
    size += UtilitySerializator::serialize(os, description_);

    return size;
}

size_t Action::deserialize(std::istream &is) {
    size_t size = 0;

    std::pair<size_t, std::string> temp_str_pair;
    temp_str_pair = UtilitySerializator::deserialize_string(is);
    size += temp_str_pair.first;
    time_ = QDateTime::fromString(QString(temp_str_pair.second.c_str()), DATETIME_FORMAT);

    temp_str_pair = UtilitySerializator::deserialize_string(is);
    size += temp_str_pair.first;
    description_ = temp_str_pair.second;

    return size;
}
