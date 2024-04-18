#include "textmessagecontent.h"

#include "constants.h"
#include "utility.h"

TextMessageContent::TextMessageContent() {

}

size_t TextMessageContent::serialize(std::ostream &os) const {
    size_t size = 0;

    size += Utility::serialize(os, text_data_);
    return size;
}

size_t TextMessageContent::deserialize(std::istream& is) {
    size_t size = 0;

    std::pair<size_t, std::string> temp_str_pair;
    temp_str_pair = Utility::deserialize_string(is);
    size += temp_str_pair.first;
    text_data_ = temp_str_pair.second;

    return size;
}

const char* TextMessageContent::get_raw_data() const {
    return text_data_.c_str();
}

int TextMessageContent::get_msg_content_type() const {
    return TEXT_MSG_TYPE;
}

void TextMessageContent::set_text(const std::string& text_data) {
    text_data_ = text_data;
}
