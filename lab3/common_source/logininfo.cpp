#include "logininfo.h"

#include "utility.h"

size_t LoginInfo::serialize(std::ostream &os) const {
    size_t size = 0;
    size += Utility::serialize(os, username_);
    size += Utility::serialize(os, email_);
    size += Utility::serialize(os, password_);

    return size;
}

size_t LoginInfo::deserialize(std::istream &is) {
    size_t size = 0;

    std::pair<size_t, std::string> pair = Utility::deserialize_string(is);
    size += pair.first;
    username_ = pair.second;

    pair = Utility::deserialize_string(is);
    size += pair.first;
    email_ = pair.second;

    pair = Utility::deserialize_string(is);
    size += pair.first;
    password_ = pair.second;

    return size;
}
