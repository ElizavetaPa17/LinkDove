#include "UserInfo.h"

#include "utility"

size_t UserInfo::serialize(std::ostream &os) {
    size_t size = status_info_.serialize(os);
    size += Utility::serialize(os, password_);

    return size;
}
