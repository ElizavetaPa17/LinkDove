#include "individualmessage.h"

#include "utility.h"

IndividualMessage::IndividualMessage(unsigned long long id)
    : IMessage(id)
    , sender_id_(0)
    , receiver_id_(0)
{

}

size_t IndividualMessage::serialize(std::ostream &os) const {
    size_t size = 0;

    size += UtilitySerializator::serialize_fundamental<unsigned long long>(os, id_);
    size += UtilitySerializator::serialize_fundamental<unsigned long long>(os, sender_id_);
    size += UtilitySerializator::serialize_fundamental<unsigned long long>(os, receiver_id_);
    size += msg_content_ptr_->serialize(os);

    return size;
}

size_t IndividualMessage::deserialize(std::istream& is) {
    size_t size = 0;

    std::pair<size_t, unsigned long long> temp_ullong_pair;
    temp_ullong_pair = UtilitySerializator::deserialize_fundamental<unsigned long long>(is);
    size += temp_ullong_pair.first;
    id_ = temp_ullong_pair.second;

    temp_ullong_pair = UtilitySerializator::deserialize_fundamental<unsigned long long>(is);
    size += temp_ullong_pair.first;
    sender_id_ = temp_ullong_pair.second;

    temp_ullong_pair = UtilitySerializator::deserialize_fundamental<unsigned long long>(is);
    size += temp_ullong_pair.first;
    receiver_id_ = temp_ullong_pair.second;

    size += msg_content_ptr_->deserialize(is);

    return size;
}

int IndividualMessage::get_msg_type() const {
    return INDIVIDUAL_MSG_TYPE;
}

std::pair<unsigned long long, unsigned long long> IndividualMessage::get_msg_edges() const {
    return std::make_pair(sender_id_, receiver_id_);
}

void IndividualMessage::set_msg_edges(unsigned long long sender_id, unsigned long long receiver_id) {
    sender_id_ = sender_id;
    receiver_id_ = receiver_id;
}
