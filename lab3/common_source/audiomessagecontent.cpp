#include "audiomessagecontent.h"

#include "utility.h"

AudioMessageContent::AudioMessageContent()
{}

size_t AudioMessageContent::serialize(std::ostream &os) const {
    size_t size = 0;

    size += UtilitySerializator::serialize(os, audio_path_);
    return size;
}

size_t AudioMessageContent::deserialize(std::istream& is) {
    size_t size = 0;

    std::pair<size_t, std::string> temp_str_pair;
    temp_str_pair = UtilitySerializator::deserialize_string(is);
    size += temp_str_pair.first;
    audio_path_ = temp_str_pair.second;

    return size;
}

const char* AudioMessageContent::get_raw_data() const {
    return audio_path_.c_str();
}

int AudioMessageContent::get_msg_content_type() const {
    return AUDIO_MSG_TYPE;
}

void AudioMessageContent::set_audio_path(std::string path) {
    audio_path_ = path;
}
