#include "imagemessagecontent.h"

#include "constants.h"
#include "utility.h"

ImageMessageContent::ImageMessageContent()
{}


size_t ImageMessageContent::serialize(std::ostream &os) const {
    size_t size = 0;

    size += UtilitySerializator::serialize(os, image_path_);
    return size;
}

size_t ImageMessageContent::deserialize(std::istream& is) {
    size_t size = 0;

    std::pair<size_t, std::string> temp_str_pair;
    temp_str_pair = UtilitySerializator::deserialize_string(is);
    size += temp_str_pair.first;
    image_path_ = temp_str_pair.second;

    return size;
}

const char* ImageMessageContent::get_raw_data() const {
    return image_path_.c_str();
}

int ImageMessageContent::get_msg_content_type() const {
    return IMAGE_MSG_TYPE;
}

void ImageMessageContent::set_image_path(std::string path) {
    image_path_ = path;
}
