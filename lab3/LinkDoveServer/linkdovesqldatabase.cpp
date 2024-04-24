#include "linkdovesqldatabase.h"

#include <iostream>
#include <QSqlError>
#include <QSqlQuery>
#include <mutex>
#include <algorithm>

#include "constants.h"
#include "individualmessage.h"
#include "textmessagecontent.h"

std::mutex modify_mutex;

LinkDoveSQLDataBase::LinkDoveSQLDataBase(const std::string &connection_name)
    : connection_name_(connection_name)
{
    setup();
}

LinkDoveSQLDataBase::~LinkDoveSQLDataBase() {
    data_base_.close();
    QSqlDatabase::removeDatabase(data_base_.databaseName());
}

void LinkDoveSQLDataBase::setup() {
    data_base_ = QSqlDatabase::addDatabase("QMYSQL", connection_name_.c_str());
    data_base_.setHostName("localhost");
    data_base_.setUserName("root");
    data_base_.setPassword("me7RKf13xZQ442");
    data_base_.setDatabaseName("LinkDoveDatabase");

    data_base_.open();

    if (!data_base_.isOpen()) {
        std::cerr << "Failed to setup database: " << data_base_.lastError().text().toStdString() << '\n';
        exit(EXIT_FAILURE);
    }

    if (!setup_tables()) {
        std::cerr << "An error occur while setting up the database.\n";
        exit(EXIT_FAILURE);
    }
}

bool LinkDoveSQLDataBase::setup_tables() {
    QSqlQuery query(data_base_);

    bool is_ok = query.exec("CREATE TABLE IF NOT EXISTS USERS "
                            "( ID MEDIUMINT UNIQUE AUTO_INCREMENT PRIMARY KEY, "
                            "  username VARCHAR(40) UNIQUE NOT NULL, "
                            "  email VARCHAR(40) UNIQUE NOT NULL, "
                            "  password VARCHAR(40) NOT NULL, "
                            "  birthday DATE NOT NULL, "
                            "  text_status VARCHAR(256), "
                            "  image MEDIUMBLOB, "
                            "  is_banned TINYINT DEFAULT 0); ");

    if (!is_ok) {
        std::cerr << "Failed to setup USERS table: " << query.lastError().text().toStdString() << '\n';
        return false;
    }

    is_ok = query.exec("CREATE TABLE IF NOT EXISTS COMPLAINTS "
                       "( ID MEDIUMINT UNIQUE AUTO_INCREMENT PRIMARY KEY, "
                       " sender_id MEDIUMINT NOT NULL, "
                       " text TEXT NOT NULL,"
                       " FOREIGN KEY(sender_id) REFERENCES USERS(ID) ON DELETE CASCADE); ");
    if (!is_ok) {
        std::cerr << "Failed to setup COMPLAINTS table: " << query.lastError().text().toStdString() << '\n';
        return false;
    }

    is_ok = query.exec("CREATE TABLE IF NOT EXISTS INDIVIDUAL_MESSAGES "
                       "( ID BIGINT UNIQUE AUTO_INCREMENT PRIMARY KEY , "
                       " sender_id MEDIUMINT NOT NULL, "
                       " receiver_id MEDIUMINT NOT NULL, "
                       " send_datetime DATETIME NOT NULL, "
                       " content_id BIGINT DEFAULT 0, "
                       " content_type ENUM('text', 'audio', 'image'), "
                       " FOREIGN KEY (sender_id) REFERENCES USERS (ID) ON DELETE CASCADE, "
                       " FOREIGN KEY (receiver_id) REFERENCES USERS (ID) ON DELETE CASCADE); ");

    if (!is_ok) {
        std::cerr << "Failed to setup INDIVIDUAL_MESSAGES table: " << query.lastError().text().toStdString() << '\n';
        return false;
    }

    is_ok = query.exec("CREATE TABLE IF NOT EXISTS IND_TEXT_MESSAGE_CONTENTS "
                       "( ID BIGINT UNIQUE AUTO_INCREMENT PRIMARY KEY, "
                       " msg_id BIGINT NOT NULL, "
                       " text_data TEXT NOT NULL, "
                       " FOREIGN KEY (msg_id) REFERENCES INDIVIDUAL_MESSAGES (ID) ON DELETE CASCADE); ");

    if (!is_ok) {
        std::cerr << "Failed to setup IND_TEXT_MESSAGE_CONTENTS table: " << query.lastError().text().toStdString() << '\n';
        return false;
    }

    return true;
}

bool LinkDoveSQLDataBase::register_user(const UserInfo& info) {
    QSqlQuery query(data_base_);
    query.prepare("INSERT INTO USERS (username, email, password, birthday, text_status, image, is_banned) "
                  "VALUES (:username, :email, :password, :birthday, :text_status, :image, :is_banned); ");

    query.bindValue(":username",    info.status_info_.username_.c_str());
    query.bindValue(":email",       info.status_info_.email_.c_str());
    query.bindValue(":password",    info.password_.c_str());
    query.bindValue(":birthday",    info.status_info_.birthday_.toString("yyyy-MM-dd"));
    query.bindValue(":text_status", info.status_info_.text_status_.c_str());
    query.bindValue(":is_banned",   0); // при создании пользователя его аккаунт не блокируется

    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString();
        return false;
    } else {
        // если вставка была успешна, то row affected > 0, иначе row affected == 0 (false).
        return query.numRowsAffected();
    }
}

bool LinkDoveSQLDataBase::login_user(const LoginInfo& info) {
    QSqlQuery query(data_base_);
    query.prepare("SELECT * FROM USERS "
                  "WHERE "
                  "username = :username AND email = :email AND password = :password; ");

    query.bindValue(":username", info.username_.c_str());
    query.bindValue(":email",    info.email_.c_str());
    query.bindValue(":password", info.password_.c_str());

    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString() << '\n';
        return false;
    } else {
        if (query.next()) {
            return true;
        } else {
            return false;
        }
    }
}

bool LinkDoveSQLDataBase::update_user(const StatusInfo& status_info) {
    StatusInfo inner_info;
    bool is_username_changed = false,
         is_email_changed    = false;
    try {
        inner_info = get_status_info(status_info.id_);
        if (inner_info.username_ != status_info.username_) {
            is_username_changed = true;
        }
        if (inner_info.email_ != status_info.email_) {
            is_email_changed = true;
        }

    } catch(std::runtime_error& ex) {
        return false;
    }

    QSqlQuery query(data_base_);

    // Если никнейм или почта меняется, нужно изменить вид запроса,
    // так как БД даже в случае установки тех же значений выдаст ошибку (из-за UNIQUE флагов для никнейма и почты)
    if (is_username_changed && is_email_changed) {
        query.prepare("UPDATE USERS "
                  "SET username = :username,"
                  "    email = :email, "
                  "    text_status = :text_status "
                  "WHERE ID = :id; ");
        query.bindValue(":username",    status_info.username_.c_str());
        query.bindValue(":email",       status_info.email_.c_str());
    } else if (is_username_changed) {
        query.prepare("UPDATE USERS "
                  "SET username = :username,"
                  "    text_status = :text_status "
                  "WHERE ID = :id; ");
        query.bindValue(":username",    status_info.username_.c_str());
    } else if (is_email_changed) {
        query.prepare("UPDATE USERS "
                  "SET email       = :email, "
                  "    text_status = :text_status "
                  "WHERE ID = :id; ");
        query.bindValue(":email",       status_info.email_.c_str());
    } else {
        query.prepare("UPDATE USERS "
                      "SET text_status = :text_status "
                      "WHERE ID = :id; ");
    }

    query.bindValue(":text_status", status_info.text_status_.c_str());
    query.bindValue(":id",          status_info.id_);

    if (!query.exec()) {
        std::cerr  << query.lastError().text().toStdString() << '\n';
        return false;
    } else {
        // если изменение было успешно, то row affected > 0, иначе row affected == 0 (false).
        int num = query.numRowsAffected();
        // Если num == 0, а поля никнейма или почты изменились, то на вход пришел дубликат.
        if (num == 0 && is_username_changed || num == 0 && is_email_changed) {
            return false;
        } else {
            return true;
        }
    }
}

bool LinkDoveSQLDataBase::ban_user(const std::string &username, bool is_ban) {
    QSqlQuery query(data_base_);
    query.prepare(" UPDATE USERS "
                  " SET "
                  "     is_banned=:is_banned "
                  " WHERE username=:username; ");

    query.bindValue(":username", username.c_str());
    query.bindValue(":is_banned", is_ban);

    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString() << '\n';
        return false;
    } else {
        // если изменение было успешно, то row affected > 0, иначе row affected == 0 (false).
        return query.numRowsAffected();
    }
}

bool LinkDoveSQLDataBase::get_user_banned_status(const std::string &username) {
    QSqlQuery query(data_base_);
    query.prepare(" SELECT * FROM USERS "
                  " WHERE username=:username; ");

    query.bindValue(":username", username.c_str());

    if (!query.exec() || !query.next()) {
        std::cerr << query.lastError().text().toStdString() << '\n';
        throw std::runtime_error("failed to check user banned status");
    } else {
        return query.value("is_banned").toBool();
    }
}

bool LinkDoveSQLDataBase::add_complaint(const Complaint& complaint) {
    QSqlQuery query(data_base_);
    query.prepare("INSERT INTO COMPLAINTS (sender_id, text) "
                  "VALUES (:sender_id, :text); ");

    query.bindValue(":sender_id", complaint.sender_id_);
    query.bindValue(":text", complaint.text_.c_str());

    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString() << '\n';
        return false;
    } else {
        // если вставка была успешна, то row affected > 0, иначе row affected == 0 (false).
        return query.numRowsAffected();
    }
}

bool LinkDoveSQLDataBase::del_complaint(unsigned long long complaint_id) {
    QSqlQuery query(data_base_);
    query.prepare("DELETE FROM COMPLAINTS "
                  "WHERE ID=:id; ");

    query.bindValue(":id", complaint_id);
    std::cerr << "id: " << complaint_id << '\n';
    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString() << '\n';
        return false;
    } else {
        // если удаление было успешным, то row affected > 0, иначе row affected == 0 (false).
        std::cerr << query.numRowsAffected() << '\n';
        return query.numRowsAffected();
    }
}

int LinkDoveSQLDataBase::get_complaints_count() {
    QSqlQuery query(data_base_);
    query.prepare("SELECT COUNT(*) FROM COMPLAINTS; ");

    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString() << '\n';
        return -1;
    } else {
        if (!query.next()) {
            std::cerr << "Can't get complaint count\n";
            return -1;
        } else {
            return query.value("COUNT(*)").toInt();
        }
    }
}

std::vector<Complaint> LinkDoveSQLDataBase::get_complaints(int count) {
    QSqlQuery query(data_base_);
    query.prepare("SELECT * FROM COMPLAINTS");

    std::vector<Complaint> complaints;
    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString() << '\n';
        throw std::runtime_error("get_complaints failed: query.exec");
    } else {
        if (!query.next()) {
            std::vector<Complaint>(); // в БД нет жалоб
        } else {
            return link_dove_database_details__::retrieve_complaints(query, count);
        }
    }
}

bool LinkDoveSQLDataBase::add_message(const IMessage& msg) {
    QSqlQuery query(data_base_);
    unsigned long long msg_id = 0;

    // используем мьютекс для того, чтобы во время транзакции не было модификации данных
    std::unique_lock<std::mutex> unique_mtx(modify_mutex);

    // используем транзакцию, чтобы в случае ошибки добавления сообщения данные в БД были
    // в согласованном состоянии
    if (data_base_.transaction()) {
        switch (msg.get_msg_type()) {
            case INDIVIDUAL_MSG_TYPE:  {
                query.prepare(" INSERT INTO INDIVIDUAL_MESSAGES "
                              " (sender_id, send_datetime, receiver_id) "
                              " VALUES (:sender_id, NOW(), :receiver_id); ");

                query.bindValue(":sender_id", static_cast<const IndividualMessage&>(msg).get_msg_edges().first);
                query.bindValue(":receiver_id", static_cast<const IndividualMessage&>(msg).get_msg_edges().second);

                if (!query.exec()) {
                    std::cerr << query.lastError().text().toStdString() << '\n';
                    return false;
                }

                query.prepare("SELECT * FROM INDIVIDUAL_MESSAGES "
                              "WHERE "
                              " sender_id=:sender_id AND receiver_id=:receiver_id AND content_id=0; ");
                query.bindValue(":sender_id", static_cast<const IndividualMessage&>(msg).get_msg_edges().first);
                query.bindValue(":receiver_id", static_cast<const IndividualMessage&>(msg).get_msg_edges().second);

                if (!query.exec()) {
                    std::cerr << query.lastError().text().toStdString() << '\n';
                    return false;
                } else {
                    if (!query.next()) {
                        std::cerr << "Can't retrieve message\n";
                        return false;
                    } else {
                        msg_id = query.value("ID").toULongLong();
                    }
                }
            }
        }

        std::string content_enum;
        unsigned long long content_id = 0;
        switch (msg.get_msg_content()->get_msg_content_type()) {
            case TEXT_MSG_TYPE: {
                query.prepare(" INSERT INTO IND_TEXT_MESSAGE_CONTENTS "
                              " (msg_id, text_data) "
                              " VALUES (:msg_id, :text_data); ");
                query.bindValue(":msg_id", msg_id);
                query.bindValue(":text_data", msg.get_msg_content()->get_raw_data());

                if (!query.exec()) {
                    std::cerr << query.lastError().text().toStdString() << '\n';
                    return false;
                }

                query.prepare(" SELECT * FROM IND_TEXT_MESSAGE_CONTENTS "
                              " WHERE msg_id = :msg_id; ");
                query.bindValue(":msg_id", msg_id);

                if (!query.exec()) {
                    std::cerr << query.lastError().text().toStdString() << '\n';
                    return false;
                } else {
                    if (!query.next()) {
                        return false;
                    } else {
                        content_id = query.value("ID").toULongLong();
                    }
                }

                content_enum = "text";
                break;
            }
        }

        switch (msg.get_msg_type()) {
            case INDIVIDUAL_MSG_TYPE:  {
                query.prepare(" UPDATE INDIVIDUAL_MESSAGES "
                              " SET content_id   = :content_id,"
                              "     content_type = :content_type "
                              " WHERE ID = :msg_id; ");

                query.bindValue(":content_id", content_id);
                query.bindValue(":content_type", content_enum.c_str());
                query.bindValue(":msg_id", msg_id);

                break;
            }
        }

        if (!query.exec()) {
            std::cerr << query.lastError().text().toStdString() << '\n';
            return false;
        }

        if (!data_base_.commit()) {
            std::cerr << "Failed to commit\n";
            return false;
        }
    } else {
        std::cerr << "Failed to start transaction\n";
        return false;
    }

    query.exec(" COMMIT; ");
    return true;
}

std::vector<std::shared_ptr<IMessage>> LinkDoveSQLDataBase::get_ind_messages(unsigned long long sender_id, unsigned long long receiver_id) {
    QSqlQuery query(data_base_);

    query.prepare("SELECT * FROM INDIVIDUAL_MESSAGES "
                  " WHERE sender_id=:sender_id and receiver_id=:receiver_id; ");

    query.bindValue(":sender_id", sender_id);
    query.bindValue(":receiver_id", receiver_id);

    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString() << '\n';
        throw std::runtime_error("get_ind_messages failed: cannot get individual messages\n");
    }

    if (!query.next()) {
        return std::vector<std::shared_ptr<IMessage>>(); // в БД нет сообщений с указанными требованиями
    } else {
        QSqlQuery content_query(data_base_);
        return link_dove_database_details__::retrieve_messages(query, content_query);
    }

}

std::vector<StatusInfo> LinkDoveSQLDataBase::get_interlocutors(unsigned long long id) {
    QSqlQuery query(data_base_);

    query.prepare(" SELECT sender_id FROM INDIVIDUAL_MESSAGES "
                  " WHERE receiver_id=:id "
                  " GROUP BY sender_id; ");

    query.bindValue(":id", id);

    std::vector<StatusInfo> interlocutors;
    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString() << '\n';
        throw std::runtime_error("get_interlocutors: cannot get info user interlocutors from theirs messages.\n");
    } else {
        while (query.next()) {
            interlocutors.push_back(get_status_info(query.value("sender_id").toULongLong()));
        }
    }

    query.prepare(" SELECT receiver_id FROM INDIVIDUAL_MESSAGES "
                  " WHERE sender_id=:id "
                  " GROUP BY receiver_id; ");

    query.bindValue(":id", id);

    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString() << '\n';
        throw std::runtime_error("get_interlocutors: cannot get info user interlocutors from theirs messages.\n");
    } else {
        while (query.next()) {
            interlocutors.push_back(get_status_info(query.value("receiver_id").toULongLong()));
        }
    }

    std::sort(interlocutors.begin(), interlocutors.end(), [this] (const StatusInfo &info1, const StatusInfo &info2) {
        return info1.id_ < info2.id_;
    });
    auto it = std::unique(interlocutors.begin(), interlocutors.end(), [this] (const StatusInfo &info1, const StatusInfo &info2) {
        return info1.id_ == info2.id_;
    });
    interlocutors.erase(it, interlocutors.end());

    return interlocutors;
}

StatusInfo LinkDoveSQLDataBase::get_status_info(const std::string &username) {
    QSqlQuery query(data_base_);
    query.prepare("SELECT * FROM USERS "
                  "WHERE "
                  "username = :username; ");

    query.bindValue(":username", username.c_str());

    if (!query.exec()) {
        throw std::runtime_error(query.lastError().text().toStdString());
    } else {
        if (!query.next()) {
            throw std::runtime_error("No such object in DataBase");
        } else {
            return link_dove_database_details__::retrieve_status_info(query);
        }
    }
}

StatusInfo LinkDoveSQLDataBase::get_status_info(unsigned long long id) {
    QSqlQuery query(data_base_);
    query.prepare("SELECT * FROM USERS "
                  "WHERE "
                  "ID = :id; ");

    query.bindValue(":id", id);

    if (!query.exec()) {
        throw std::runtime_error(query.lastError().text().toStdString());
    } else {
        if (!query.next()) {
            throw std::runtime_error("No such object in DataBase");
        } else {
            return link_dove_database_details__::retrieve_status_info(query);
        }
    }
}

namespace link_dove_database_details__ {
    StatusInfo retrieve_status_info(const QSqlQuery& query) {
        StatusInfo status_info;
        status_info.id_          = query.value("ID").toULongLong();
        status_info.username_    = query.value("username").toString().toStdString();
        status_info.email_       = query.value("email").toString().toStdString();
        status_info.birthday_    = QDate::fromString(query.value("birthday").toString(), QString(BIRTHAY_FORMAT));
        status_info.text_status_ = query.value("text_status").toString().toStdString();

        QByteArray image_bytes   = query.value("image").toByteArray();
        status_info.image_bytes_ = std::vector(image_bytes.begin(), image_bytes.end());

        return status_info;
    }

    std::vector<Complaint> retrieve_complaints(QSqlQuery& query, int count) {
        std::vector<Complaint> complaints;
        Complaint complaint;

        do {
            complaint.id_ = query.value("ID").toInt();
            complaint.sender_id_ = query.value("sender_id").toInt();
            complaint.text_ = query.value("text").toString().toStdString();

            complaints.push_back(complaint);
            --count;

        } while (query.next() && count);

        return complaints;
    }

    std::vector<std::shared_ptr<IMessage>> retrieve_messages(QSqlQuery &query, QSqlQuery &content_query) {
        if (query.size() < 0) {
            throw std::runtime_error("retrieve_messages failed: query size is negative.\n");
        }

        std::vector<std::shared_ptr<IMessage>> messages;
        std::string msg_type;
        unsigned long long sender_id = query.value("sender_id").toULongLong(),
                           receiver_id = query.value("receiver_id").toULongLong();


        do {
            std::shared_ptr<IndividualMessage> message_ptr = std::make_shared<IndividualMessage>();
            message_ptr->set_id(query.value("ID").toULongLong());
            message_ptr->set_send_datetime(query.value("send_datetime").toDateTime());
            message_ptr->set_msg_edges(sender_id, receiver_id);

            msg_type = query.value("content_type").toString().toStdString();
            if (msg_type == "text") {
                content_query.prepare("SELECT * FROM IND_TEXT_MESSAGE_CONTENTS "
                                      "WHERE msg_id=:msg_id; ");

                content_query.bindValue(":msg_id", message_ptr->get_id());

                if (!content_query.exec() || !content_query.next()) {
                    std::cerr << content_query.lastError().text().toStdString() << '\n';
                    continue;
                }

                std::shared_ptr<TextMessageContent> text_msg_content_ptr = std::make_shared<TextMessageContent>();
                text_msg_content_ptr->set_text(content_query.value("text_data").toString().toStdString());
                message_ptr->set_msg_content(text_msg_content_ptr);

            } else if (msg_type == "audio") {
                // TODO
            } else if (msg_type == "image") {
                // TODO
            }

            messages.push_back(message_ptr);
        } while (query.next());

        return messages;
    }
}
