#include "complaint.h"

#include <utility>

#include "utility.h"

size_t Complaint::serialize(std::ostream& os) const {
    size_t size = 0;

    size += Utility::serialize_fundamental<unsigned long long>(os, id_);
    size += Utility::serialize_fundamental<unsigned long long>(os, sender_id_);
    size += Utility::serialize(os, text_);

    return size;
}

size_t Complaint::deserialize(std::istream &is) {
    size_t size = 0;

    std::pair<size_t, unsigned long long> temp_ullong_pair;
    temp_ullong_pair = Utility::deserialize_fundamental<unsigned long long>(is);
    size += temp_ullong_pair.first;
    id_ = temp_ullong_pair.second;

    temp_ullong_pair = Utility::deserialize_fundamental<unsigned long long>(is);
    size += temp_ullong_pair.first;
    sender_id_ = temp_ullong_pair.second;

    std::pair<size_t, std::string> temp_str_pair;
    temp_str_pair = Utility::deserialize_string(is);

    return size;
}
