#ifndef AUDIOMESSAGECONTENT_H
#define AUDIOMESSAGECONTENT_H

#include <string>
#include "imessagecontent.h"

class AudioMessageContent : public IMessageContent
{
public:
    AudioMessageContent();

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
     * <p> Устанавливает путь к аудио и перемещает ее в папку сборки - media/audio. </p>
     * @brief set_image_path
     * @param path - Путь к аудио.
     */
    void set_audio_path(std::string path);

private:
    std::string audio_path_;
};

#endif // AUDIOMESSAGECONTENT_H
