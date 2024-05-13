#include "imessage.h"

#include "constants.h"

IMessage::IMessage()
    : id_(CREATED_ENTITY_ID)
    , send_datetime_(QDateTime::currentDateTime())
{

}

IMessage::~IMessage() {

}

unsigned long long IMessage::get_id() const {
    return id_;
}

void IMessage::set_id(unsigned long long id) {
    id_ = id;
}

void IMessage::set_msg_content(const std::shared_ptr<IMessageContent>& msg_content_ptr) {
    msg_content_ptr_ = msg_content_ptr;
}

const std::shared_ptr<IMessageContent> IMessage::get_msg_content() const {
    return msg_content_ptr_;
}

QDateTime IMessage::get_send_datetime() const {
    return send_datetime_;
}

void IMessage::set_send_datetime(const QDateTime& datetime) {
    send_datetime_ = datetime;
}
