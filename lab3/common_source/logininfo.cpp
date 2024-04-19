#include "logininfo.h"

#include "utility.h"

size_t LoginInfo::serialize(std::ostream &os) const {
    size_t size = 0;
    size += UtilitySerializator::serialize(os, username_);
    size += UtilitySerializator::serialize(os, email_);
    size += UtilitySerializator::serialize(os, password_);

    return size;
}

size_t LoginInfo::deserialize(std::istream &is) {
    size_t size = 0;

    std::pair<size_t, std::string> pair = UtilitySerializator::deserialize_string(is);
    size += pair.first;
    username_ = pair.second;

    pair = UtilitySerializator::deserialize_string(is);
    size += pair.first;
    email_ = pair.second;

    pair = UtilitySerializator::deserialize_string(is);
    size += pair.first;
    password_ = pair.second;

    return size;
}
