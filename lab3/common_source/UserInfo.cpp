#include "UserInfo.h"

#include "utility.h"

size_t UserInfo::serialize(std::ostream &os) const {
    size_t size = status_info_.serialize(os);
    size += Utility::serialize(os, password_);

    return size;
}

size_t UserInfo::deserialize(std::istream &is) {
    size_t size = status_info_.deserialize(is);

    std::pair<size_t, std::string> pair = Utility::deserialize_string(is);
    size += pair.first;
    password_ = pair.second;

    return size;
}
