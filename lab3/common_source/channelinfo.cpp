#include "channelinfo.h"

#include "utility.h"

size_t ChannelInfo::serialize(std::ostream& os) const {
    size_t size = 0;

    size += UtilitySerializator::serialize_fundamental<unsigned long long>(os, id_);
    size += UtilitySerializator::serialize_fundamental<unsigned long long>(os, owner_id_);
    size += UtilitySerializator::serialize(os, name_);
    size += UtilitySerializator::serialize_fundamental<bool>(os, is_banned_);

    return size;
}

size_t ChannelInfo::deserialize(std::istream& is) {
    size_t size = 0;

    std::pair<size_t, unsigned long long> temp_ullong_pair;
    temp_ullong_pair = UtilitySerializator::deserialize_fundamental<unsigned long long>(is);
    size += temp_ullong_pair.first;
    id_ = temp_ullong_pair.second;

    temp_ullong_pair = UtilitySerializator::deserialize_fundamental<unsigned long long>(is);
    size += temp_ullong_pair.first;
    owner_id_ = temp_ullong_pair.second;

    std::pair<size_t, std::string> temp_str_pair;
    temp_str_pair = UtilitySerializator::deserialize_string(is);
    size += temp_str_pair.first;
    name_ = temp_str_pair.second;

    std::pair<size_t, bool> temp_bool_pair;
    temp_bool_pair = UtilitySerializator::deserialize_fundamental<bool>(is);
    size += temp_bool_pair.first;
    is_banned_ = temp_bool_pair.second;

    return size;
}
