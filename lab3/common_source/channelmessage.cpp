#include "channelmessage.h"

#include "utility.h"
#include "constants.h"

ChannelMessage::ChannelMessage()
    : channel_id_(CREATED_ENTITY_ID)
{

}

size_t ChannelMessage::serialize(std::ostream &os) const {
    size_t size = 0;

    size += UtilitySerializator::serialize_fundamental<unsigned long long>(os, id_);
    size += UtilitySerializator::serialize(os, send_datetime_.toString(DATETIME_FORMAT).toStdString());
    size += UtilitySerializator::serialize_fundamental<unsigned long long>(os, channel_id_);
    size += msg_content_ptr_->serialize(os);

    return size;
}

size_t ChannelMessage::deserialize(std::istream& is) {
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
    channel_id_ = temp_ullong_pair.second;

    size += msg_content_ptr_->deserialize(is);

    return size;
}

int ChannelMessage::get_msg_type() const {
    return CHANNEL_MSG_TYPE;
}

void ChannelMessage::set_channel_id(unsigned long long channel_id) {
    channel_id_ = channel_id;
}

unsigned long long ChannelMessage::get_channel_id() const {
    return channel_id_;
}
