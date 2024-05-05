#ifndef GROUPMESSAGE_H
#define GROUPMESSAGE_H

#include "imessage.h"
#include "imessagecontent.h"

/**
 * @brief The GroupMessage class
 * Класс сообщений в группе.
 */
class GroupMessage : public IMessage
{
public:
    GroupMessage();

    size_t serialize(std::ostream &os) const override;

    size_t deserialize(std::istream& is) override;

    int get_msg_type() const override;

    /**
     * <p> Устанавливает идентификатор группы. </p>
     * @brief set_group_id
     * @param group_id - Идентификатор группы, которой принадлежит сообщение.
     */
    void set_group_id(unsigned long long group_id);

    /**
     * <p> Возвращает идентификатор группы. </p>
     * @brief get_group_id
     * @return - Идентификатор группы, которой принадлежит сообщение.
     */
    unsigned long long get_group_id() const;

    /**
     * <p> Устанавливает идентификатор владельца сообщения. </p>
     * @brief set_owner_id
     * @param owner_id - Идентификатор владельца сообщения.
     */
    void set_owner_id(unsigned long long owner_id);

    /**
     * <p> Возвращает идентификатор владельца сообщения. </p>
     * @brief get_owner_id
     * @return - Идентификатор владельца сообщения.
     */
    unsigned long long get_owner_id() const;

    /**
     * <p> Устанавливает никнейм отправителя. </p>
     * @brief set_owner_name
     * @param owner_name - Никнейм отправителя.
     */
    void set_owner_name(const std::string &owner_name);

    /**
     * <p> Возвращает никнейм отправителя. </p>
     * @brief get_owner_name
     * @return - Никнейм отправителя.
     */
    std::string get_owner_name() const;

private:
    unsigned long long group_id_;
    unsigned long long owner_id_;
    std::string owner_name_; // Устанавливается в БД
};

#endif // GROUPMESSAGE_H
