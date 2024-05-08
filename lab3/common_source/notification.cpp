#include "notification.h"


#include <utility>
#include "utility.h"

size_t Notification::serialize(std::ostream& os) const {
    size_t size = 0;

    size += UtilitySerializator::serialize_fundamental<unsigned long long>(os, id_);
    size += UtilitySerializator::serialize_fundamental<unsigned long long>(os, receiver_id_);
    size += UtilitySerializator::serialize(os, text_);

    return size;
}

size_t Notification::deserialize(std::istream &is) {
    size_t size = 0;

    std::pair<size_t, unsigned long long> temp_ullong_pair;
    temp_ullong_pair = UtilitySerializator::deserialize_fundamental<unsigned long long>(is);
    size += temp_ullong_pair.first;
    id_ = temp_ullong_pair.second;

    temp_ullong_pair = UtilitySerializator::deserialize_fundamental<unsigned long long>(is);
    size += temp_ullong_pair.first;
    receiver_id_ = temp_ullong_pair.second;

    std::pair<size_t, std::string> temp_str_pair;
    temp_str_pair = UtilitySerializator::deserialize_string(is);
    size += temp_str_pair.first;
    text_ = temp_str_pair.second;

    return size;
}
