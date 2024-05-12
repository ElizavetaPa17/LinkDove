#ifndef IMAGEMESSAGECONTENT_H
#define IMAGEMESSAGECONTENT_H

#include <string>
#include "imessagecontent.h"

class ImageMessageContent : public IMessageContent
{
public:
    ImageMessageContent();

    size_t serialize(std::ostream &os) const override;

    size_t deserialize(std::istream& is) override;

    /**
     * <p> Возвращает указатель на данные, хранящиеся в сообщении. </p>
     * @brief get_raw_data
     * @return - Указатель на данные.
     */
    const char* get_raw_data() const override;

    /**
     * <p> Возвращает тип содержимого сообщения. </p>
     * @brief get_msg_type
     * @return - Тип сообщения (enum MessageContentType).
     */
    int get_msg_content_type() const override;

    /**
     * <p> Устанавливает путь к картинке и перемещает ее в папку сборки - media/images. </p>
     * @brief set_image_path
     * @param path - Путь к картинке.
     */
    void set_image_path(std::string path);

private:
    std::string image_path_;
};

#endif // IMAGEMESSAGECONTENT_H
