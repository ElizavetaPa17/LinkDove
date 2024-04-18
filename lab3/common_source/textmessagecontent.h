#ifndef TEXTMESSAGECONTENT_H
#define TEXTMESSAGECONTENT_H

#include <string>

#include "imessagecontent.h"

class TextMessageContent : public IMessageContent
{
public:
    TextMessageContent();

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
     * @return - Тип сообщения (enum MessageContentType=TEXT_MSG).
     */
    int get_msg_content_type() const override;

    /**
     * <p> Устанавливает текст в содержимое сообщения. </p>
     * @brief set_text
     * @param text_data
     */
    void set_text(const std::string& text_data);

private:
    std::string text_data_;
};

#endif // TEXTMESSAGECONTENT_H
