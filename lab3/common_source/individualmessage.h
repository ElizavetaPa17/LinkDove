#ifndef INDIVIDUALMESSAGE_H
#define INDIVIDUALMESSAGE_H

#include "imessage.h"
#include "imessagecontent.h"
#include "memory"

/**
 * @brief The IndividualMessage class
 * Класс сообщения между двумя пользователями.
 */
class IndividualMessage : public IMessage
{
public:
    IndividualMessage();

    size_t serialize(std::ostream &os) const override;

    size_t deserialize(std::istream& is) override;

    int get_msg_type() const override;

    /**
     * <p> Возвращает пару идентификаторов отправителя и получателя. </p>
     * @brief get_msg_edges
     * @return - Пара <идентификатор отправителя, идентификатор получателя>
     */
    std::pair<unsigned long long, unsigned long long> get_msg_edges() const;

    /**
     * <p> Устанавливает пару идентификаторов пользователей: отправителя и получателя. <p>
     * @brief set_msg_edges
     * @param sender_id - Идентификатор отправителя.
     * @param receiver_id - Идентификатор получателя.
     */
    void set_msg_edges(unsigned long long sender_id, unsigned long long receiver_id);

private:
    unsigned long long sender_id_;
    unsigned long long receiver_id_;
};

#endif // INDIVIDUALMESSAGE_H
