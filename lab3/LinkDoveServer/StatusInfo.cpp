#include "StatusInfo.h"

#include "utility.h"

size_t StatusInfo::serialize(std::ostream &os) {
    size_t size = 0;

    size += Utility::serialize(os, username_);
    size += Utility::serialize(os, email_);
    size += Utility::serialize(os, birthday_.toString("d:M:yyyy").toStdString());
    size += Utility::serialize(os, text_status_);
    size += Utility::serialize(os, image_bytes_);

    return size;
}
