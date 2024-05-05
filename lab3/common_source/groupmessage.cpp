#include "groupmessage.h"

#include "utility.h"
#include "constants.h"

GroupMessage::GroupMessage()
    : group_id_(CREATED_ENTITY_ID)
{

}


size_t GroupMessage::serialize(std::ostream &os) const {
    size_t size = 0;

    size += UtilitySerializator::serialize_fundamental<unsigned long long>(os, id_);
    size += UtilitySerializator::serialize(os, send_datetime_.toString(DATETIME_FORMAT).toStdString());
    size += UtilitySerializator::serialize_fundamental<unsigned long long>(os, group_id_);
    size += UtilitySerializator::serialize_fundamental<unsigned long long>(os, owner_id_);
    size += UtilitySerializator::serialize(os, owner_name_);
    size += msg_content_ptr_->serialize(os);

    return size;
}

size_t GroupMessage::deserialize(std::istream& is) {
    size_t size = 0;

    std::pair<size_t, unsigned long long> temp_ullong_pair;
    temp_ullong_pair = UtilitySerializator::deserialize_fundamental<unsigned long long>(is);
    size += temp_ullong_pair.first;
    id_ = temp_ullong_pair.second;

    std::pair<size_t, std::string> temp_str_pair;
    temp_str_pair = UtilitySerializator::deserialize_string(is);
    size += temp_str_pair.first;
    send_datetime_ = QDateTime::fromString(QString(temp_str_pair.second.c_str()), DATETIME_FORMAT);

    temp_ullong_pair = UtilitySerializator::deserialize_fundamental<unsigned long long>(is);
    size += temp_ullong_pair.first;
    group_id_ = temp_ullong_pair.second;

    temp_ullong_pair = UtilitySerializator::deserialize_fundamental<unsigned long long>(is);
    size += temp_ullong_pair.first;
    owner_id_ = temp_ullong_pair.second;

    temp_str_pair = UtilitySerializator::deserialize_string(is);
    size += temp_str_pair.first;
    owner_name_ = temp_str_pair.second;

    size += msg_content_ptr_->deserialize(is);

    return size;
}

int GroupMessage::get_msg_type() const {
    return GROUP_MSG_TYPE;
}

void GroupMessage::set_group_id(unsigned long long group_id) {
    group_id_ = group_id;
}

unsigned long long GroupMessage::get_group_id() const {
    return group_id_;
}

void GroupMessage::set_owner_id(unsigned long long owner_id) {
    owner_id_ = owner_id;
}

unsigned long long GroupMessage::get_owner_id() const {
    return owner_id_;
}

void GroupMessage::set_owner_name(const std::string &owner_name) {
    owner_name_ = owner_name;
}

std::string GroupMessage::get_owner_name() const {
    return owner_name_;
}
