#include "imessage.h"

IMessage::IMessage(unsigned long long id)
    : id_(id)
{

}

IMessage::~IMessage() {

}

unsigned long long IMessage::get_id() {
    return id_;
}

void IMessage::set_msg_content(const std::shared_ptr<IMessageContent>& msg_content_ptr) {
    msg_content_ptr_ = msg_content_ptr;
}

const std::shared_ptr<IMessageContent> IMessage::get_msg_content() const {
    return msg_content_ptr_;
}
