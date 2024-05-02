#ifndef CHANNELMESSAGE_H
#define CHANNELMESSAGE_H

#include "imessage.h"
#include "imessagecontent.h"

/**
 * @brief The ChannelMessage class
 * Класс сообщений в канале.
 */
class ChannelMessage : public IMessage
{
public:
    ChannelMessage();

    size_t serialize(std::ostream &os) const override;

    size_t deserialize(std::istream& is) override;

    int get_msg_type() const override;

    /**
     * <p> Устанавливает идентификатор канала. </p>
     * @brief set_channel_id
     * @param channel_id - Идентификатор канала, которому принадлежит сообщение.
     */
    void set_channel_id(unsigned long long channel_id);

    /**
     * <p> Возвращает идентификатор канала. </p>
     * @brief get_channel_id
     * @return - Идентификатор канала, которому принадлежит сообщение.
     */
    unsigned long long get_channel_id() const;

private:
    unsigned long long channel_id_;
};

#endif // CHANNELMESSAGE_H
