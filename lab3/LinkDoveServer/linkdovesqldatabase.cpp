#include "linkdovesqldatabase.h"

#include <iostream>
#include <QSqlError>
#include <QSqlQuery>
#include <mutex>
#include <algorithm>

#include "constants.h"
#include "individualmessage.h"
#include "textmessagecontent.h"
#include "imagemessagecontent.h"
#include "audiomessagecontent.h"
#include "channelmessage.h"
#include "groupmessage.h"

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
                            "  image VARCHAR(400) DEFAULT 'media/avatars/default_avatar.png', "
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

    is_ok = query.exec("CREATE TABLE IF NOT EXISTS ANSWERS "
                       "( ID BIGINT UNIQUE AUTO_INCREMENT PRIMARY KEY, "
                       "  receiver_id MEDIUMINT NOT NULL, "
                       "  text TEXT NOT NULL, "
                       "  FOREIGN KEY (receiver_id) REFERENCES USERS (ID) ON DELETE CASCADE); ");

    if (!is_ok) {
        std::cerr << "Failed to setup ANSWERS table: " << query.lastError().text().toStdString() << '\n';
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

    is_ok = query.exec("CREATE TABLE IF NOT EXISTS IND_IMAGE_MESSAGE_CONTENTS "
                       "( ID BIGINT UNIQUE AUTO_INCREMENT PRIMARY KEY, "
                       " msg_id BIGINT NOT NULL, "
                       " image_path TEXT NOT NULL, "
                       " FOREIGN KEY (msg_id) REFERENCES INDIVIDUAL_MESSAGES (ID) ON DELETE CASCADE); ");

    if (!is_ok) {
        std::cerr << "Failed to setup IND_IMAGE_MESSAGE_CONTENTS table: " << query.lastError().text().toStdString() << '\n';
        return false;
    }

    is_ok = query.exec("CREATE TABLE IF NOT EXISTS IND_AUDIO_MESSAGE_CONTENTS "
                       "( ID BIGINT UNIQUE AUTO_INCREMENT PRIMARY KEY, "
                       " msg_id BIGINT NOT NULL, "
                       " audio_path TEXT NOT NULL, "
                       " FOREIGN KEY (msg_id) REFERENCES INDIVIDUAL_MESSAGES (ID) ON DELETE CASCADE); ");

    if (!is_ok) {
        std::cerr << "Failed to setup IND_AUDIO_MESSAGE_CONTENTS table: " << query.lastError().text().toStdString() << '\n';
    }

    is_ok = query.exec(" CREATE TABLE IF NOT EXISTS INTERLOCUTOR_BANNED_USERS "
                       "( ID BIGINT UNIQUE AUTO_INCREMENT PRIMARY KEY, "
                       " from_id MEDIUMINT NOT NULL, "
                       " to_id MEDIUMINT NOT NULL, "
                       " FOREIGN KEY (from_id) REFERENCES USERS (ID) ON DELETE CASCADE, "
                       " FOREIGN KEY (to_id) REFERENCES USERS (ID) ON DELETE CASCADE); ");

    if (!is_ok) {
        std::cerr << "Failed to setup INTERLOCUTOR_BANNED_USERS: " << query.lastError().text().toStdString() << '\n';
        return false;
    }

    is_ok = query.exec("CREATE TABLE IF NOT EXISTS CHANNELS "
                       "( ID BIGINT UNIQUE AUTO_INCREMENT PRIMARY KEY, "
                       " owner_id MEDIUMINT NOT NULL, "
                       " name VARCHAR(64) UNIQUE NOT NULL, "
                       " is_private TINYINT DEFAULT 0, "
                       " FOREIGN KEY (owner_id) REFERENCES USERS(ID) ON DELETE CASCADE); ");

    if (!is_ok) {
        std::cerr << "Failed to setup CHANNELS table: " << query.lastError().text().toStdString() << '\n';
        return false;
    }

    is_ok = query.exec(" CREATE TABLE IF NOT EXISTS CHANNEL_PARTICIPANTS "
                       " (ID BIGINT UNIQUE AUTO_INCREMENT PRIMARY KEY, "
                       " channel_id BIGINT NOT NULL, "
                       " participant_id MEDIUMINT NOT NULL, "
                       " FOREIGN KEY (channel_id) REFERENCES CHANNELS(ID) ON DELETE CASCADE, "
                       " FOREIGN KEY (participant_id) REFERENCES USERS(ID) ON DELETE CASCADE); ");

    if (!is_ok) {
        std::cerr << "Failed to setup CHANNEL_PARTICIPANTS table: " << query.lastError().text().toStdString() << '\n';
        return false;
    }

    is_ok = query.exec(" CREATE TABLE IF NOT EXISTS CHANNEL_REQUESTS "
                       " (ID BIGINT UNIQUE AUTO_INCREMENT PRIMARY KEY, "
                       " channel_id BIGINT NOT NULL, "
                       " user_id MEDIUMINT NOT NULL, "
                       " FOREIGN KEY (channel_id) REFERENCES CHANNELS(ID) ON DELETE CASCADE, "
                       " FOREIGN KEY (user_id) REFERENCES USERS(ID) ON DELETE CASCADE); "
                       " ALTER TABLE CHANNEL_REQUESTS ADD UNIQUE unique_index (channel_id, user_id); ");

    if (!is_ok) {
        std::cerr << "Failed to setup CHANNEL_REQUESTS table: " << query.lastError().text().toStdString() << '\n';
        return false;
    }

    is_ok = query.exec(" CREATE TABLE IF NOT EXISTS CHANNEL_MESSAGES "
                       "( ID BIGINT UNIQUE AUTO_INCREMENT PRIMARY KEY, "
                       " channel_id BIGINT NOT NULL, "
                       " send_datetime DATETIME NOT NULL, "
                       " content_id BIGINT DEFAULT 0, "
                       " content_type ENUM('text', 'audio', 'image'), "
                       " FOREIGN KEY (channel_id) REFERENCES CHANNELS (ID) ON DELETE CASCADE); ");

    if (!is_ok) {
        std::cerr << "Failed to setup CHANNEL_MESSAGES table: " << query.lastError().text().toStdString() << '\n';
        return false;
    }

    is_ok = query.exec(" CREATE TABLE IF NOT EXISTS CHANNEL_TEXT_MESSAGE_CONTENTS "
                       "( ID BIGINT UNIQUE AUTO_INCREMENT PRIMARY KEY, "
                       " msg_id BIGINT NOT NULL, "
                       " text_data TEXT NOT NULL, "
                       " FOREIGN KEY (msg_id) REFERENCES CHANNEL_MESSAGES (ID) ON DELETE CASCADE); ");

    if (!is_ok) {
        std::cerr << "Failed to setup CHANNEL_TEXT_MESSAGE_CONTENTS table: " << query.lastError().text().toStdString() << '\n';
        return false;
    }

    is_ok = query.exec(" CREATE TABLE IF NOT EXISTS CHANNEL_IMAGE_MESSAGE_CONTENTS "
                       "( ID BIGINT UNIQUE AUTO_INCREMENT PRIMARY KEY, "
                       " msg_id BIGINT NOT NULL, "
                       " image_path TEXT NOT NULL, "
                       " FOREIGN KEY (msg_id) REFERENCES CHANNEL_MESSAGES (ID) ON DELETE CASCADE); ");

    if (!is_ok) {
        std::cerr << "Failed to setup CHANNEL_IMAGE_MESSAGE_CONTENTS table: " << query.lastError().text().toStdString() << '\n';
        return false;
    }

    is_ok = query.exec(" CREATE TABLE IF NOT EXISTS CHANNEL_AUDIO_MESSAGE_CONTENTS "
                       "( ID BIGINT UNIQUE AUTO_INCREMENT PRIMARY KEY, "
                       " msg_id BIGINT NOT NULL, "
                       " audio_path TEXT NOT NULL, "
                       " FOREIGN KEY (msg_id) REFERENCES CHANNEL_MESSAGES (ID) ON DELETE CASCADE); ");

    if (!is_ok) {
        std::cerr << "Failed to setup CHANNEL_AUDIO_MESSAGE_CONTENTS table: " << query.lastError().text().toStdString() << '\n';
        return false;
    }

    is_ok = query.exec(" CREATE TABLE IF NOT EXISTS CHANNEL_BANNED_USERS "
                       "( ID BIGINT UNIQUE AUTO_INCREMENT PRIMARY KEY, "
                       " user_id MEDIUMINT NOT NULL, "
                       " channel_id BIGINT NOT NULL, "
                       " FOREIGN KEY (user_id) REFERENCES USERS (ID) ON DELETE CASCADE, "
                       " FOREIGN KEY (channel_id) REFERENCES CHANNELS (ID) ON DELETE CASCADE); ");

    if (!is_ok) {
        std::cerr << "Failed to setup CHANNEL_BANNED_USERS: " << query.lastError().text().toStdString() << '\n';
        return false;
    }

    is_ok = query.exec(" CREATE TABLE IF NOT EXISTS CHATS "
                       "( ID BIGINT UNIQUE AUTO_INCREMENT PRIMARY KEY, "
                       " owner_id MEDIUMINT NOT NULL, "
                       " name VARCHAR(64) UNIQUE NOT NULL, "
                       " is_private TINYINT DEFAULT 0, "
                       " FOREIGN KEY (owner_id) REFERENCES USERS(ID) ON DELETE CASCADE); ");

    if (!is_ok) {
        std::cerr << "Failed to setup CHATS table: " << query.lastError().text().toStdString() << '\n';
        return false;
    }

    is_ok = query.exec(" CREATE TABLE IF NOT EXISTS CHAT_PARTICIPANTS "
                       "( ID BIGINT UNIQUE AUTO_INCREMENT PRIMARY KEY, "
                       " chat_id BIGINT NOT NULL, "
                       " participant_id MEDIUMINT NOT NULL, "
                       " FOREIGN KEY (chat_id) REFERENCES CHATS(ID) ON DELETE CASCADE, "
                       " FOREIGN KEY (participant_id) REFERENCES USERS(ID) ON DELETE CASCADE); ");

    if (!is_ok) {
        std::cerr << "Failed to setup CHAT_PARTICIPANT table: " << query.lastError().text().toStdString() << '\n';
        return false;
    }

    is_ok = query.exec(" CREATE TABLE IF NOT EXISTS CHAT_REQUESTS "
                       " (ID BIGINT UNIQUE AUTO_INCREMENT PRIMARY KEY, "
                       " chat_id BIGINT NOT NULL, "
                       " user_id MEDIUMINT NOT NULL, "
                       " FOREIGN KEY (chat_id) REFERENCES CHATS(ID) ON DELETE CASCADE, "
                       " FOREIGN KEY (user_id) REFERENCES USERS(ID) ON DELETE CASCADE); "
                       " ALTER TABLE CHAT_REQUESTS ADD UNIQUE unique_index (chat_id, user_id); ");

    if (!is_ok) {
        std::cerr << "Failed to setup CHAT_REQUESTS table: " << query.lastError().text().toStdString() << '\n';
        return false;
    }

    is_ok = query.exec(" CREATE TABLE IF NOT EXISTS CHAT_MESSAGES "
                       "( ID BIGINT UNIQUE AUTO_INCREMENT PRIMARY KEY, "
                       " chat_id BIGINT NOT NULL, "
                       " owner_id MEDIUMINT NOT NULL, "
                       " send_datetime DATETIME NOT NULL, "
                       " content_id BIGINT DEFAULT 0, "
                       " content_type ENUM('text', 'audio', 'image'), "
                       " FOREIGN KEY (owner_id) REFERENCES USERS (ID) ON DELETE CASCADE, "
                       " FOREIGN KEY (chat_id) REFERENCES CHATS (ID) ON DELETE CASCADE); ");

    if (!is_ok) {
        std::cerr << "Failed to setup CHAT_MESSAGES table: " << query.lastError().text().toStdString() << '\n';
        return false;
    }

    is_ok = query.exec(" CREATE TABLE IF NOT EXISTS CHAT_TEXT_MESSAGE_CONTENTS "
                       "( ID BIGINT UNIQUE AUTO_INCREMENT PRIMARY KEY, "
                       " msg_id BIGINT NOT NULL, "
                       " text_data TEXT NOT NULL, "
                       " FOREIGN KEY (msg_id) REFERENCES CHAT_MESSAGES (ID) ON DELETE CASCADE); ");

    if (!is_ok) {
        std::cerr << "Failed to setup CHAT_TEXT_MESSAGE_CONTENTS table: " << query.lastError().text().toStdString() << '\n';
        return false;
    }

    is_ok = query.exec(" CREATE TABLE IF NOT EXISTS CHAT_IMAGE_MESSAGE_CONTENTS "
                       "( ID BIGINT UNIQUE AUTO_INCREMENT PRIMARY KEY, "
                       " msg_id BIGINT NOT NULL, "
                       " image_path TEXT NOT NULL, "
                       " FOREIGN KEY (msg_id) REFERENCES CHAT_MESSAGES (ID) ON DELETE CASCADE); ");

    if (!is_ok) {
        std::cerr << "Failed to setup CHAT_IMAGE_MESSAGE_CONTENTS table: " << query.lastError().text().toStdString() << '\n';
        return false;
    }

    is_ok = query.exec(" CREATE TABLE IF NOT EXISTS CHAT_AUDIO_MESSAGE_CONTENTS "
                       "( ID BIGINT UNIQUE AUTO_INCREMENT PRIMARY KEY, "
                       " msg_id BIGINT NOT NULL, "
                       " audio_path TEXT NOT NULL, "
                       " FOREIGN KEY (msg_id) REFERENCES CHAT_MESSAGES (ID) ON DELETE CASCADE); ");

    if (!is_ok) {
        std::cerr << "Failed to setup CHAT_AUDIO_MESSAGE_CONTENTS table: " << query.lastError().text().toStdString() << '\n';
        return false;
    }

    is_ok = query.exec(" CREATE TABLE IF NOT EXISTS CHAT_BANNED_USERS "
                       "( ID BIGINT UNIQUE AUTO_INCREMENT PRIMARY KEY, "
                       " user_id MEDIUMINT NOT NULL, "
                       " chat_id BIGINT NOT NULL, "
                       " FOREIGN KEY (user_id) REFERENCES USERS (ID) ON DELETE CASCADE, "
                       " FOREIGN KEY (chat_id) REFERENCES CHATS (ID) ON DELETE CASCADE); ");

    if (!is_ok) {
        std::cerr << "Failed to setup CHAT_BANNED_USERS: " << query.lastError().text().toStdString() << '\n';
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
    query.bindValue(":image",       DEFAULT_AVATAR_PATH);

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
                  "    text_status = :text_status, "
                  "    image = :image "
                  "WHERE ID = :id; ");
        query.bindValue(":username",    status_info.username_.c_str());
        query.bindValue(":email",       status_info.email_.c_str());
    } else if (is_username_changed) {
        query.prepare("UPDATE USERS "
                  "SET username = :username,"
                  "    text_status = :text_status, "
                  "    image = :image "
                  "WHERE ID = :id; ");
        query.bindValue(":username",    status_info.username_.c_str());
    } else if (is_email_changed) {
        query.prepare("UPDATE USERS "
                  "SET email       = :email, "
                  "    text_status = :text_status, "
                  "    image = :image "
                  "WHERE ID = :id; ");
        query.bindValue(":email",       status_info.email_.c_str());
        query.bindValue(":image", status_info.image_path_.c_str());
    } else {
        query.prepare("UPDATE USERS "
                      "SET text_status = :text_status, "
                      "    image = :image "
                      "WHERE ID = :id; ");
    }

    query.bindValue(":text_status", status_info.text_status_.c_str());
    query.bindValue(":image", status_info.image_path_.c_str());
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
        // если блокировка была успешна, то row affected > 0, иначе row affected == 0 (false).
        return query.numRowsAffected();
    }
}

bool LinkDoveSQLDataBase::ban_ind_user(unsigned long long from_id, unsigned long long to_id, bool is_ban) {
    QSqlQuery query(data_base_);

    if (is_ban) {
        query.prepare(" INSERT INTO INTERLOCUTOR_BANNED_USERS "
                      " (from_id, to_id) "
                      " VALUES (:from_id, :to_id); ");
    } else {
        query.prepare(" DELETE FROM INTERLOCUTOR_BANNED_USERS "
                      " WHERE from_id=:from_id AND to_id=:to_id; ");
    }

    query.bindValue(":from_id", from_id);
    query.bindValue(":to_id", to_id);

    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString() << '\n';
        return false;
    } else {
        // если блокировка была успешна, то row affected > 0, иначе row affected == 0 (false).
        return query.numRowsAffected();
    }
}

bool LinkDoveSQLDataBase::ban_chat_user(unsigned long long chat_id, unsigned long long user_id, bool is_ban) {
    QSqlQuery query(data_base_);

    if (is_ban) {
        query.prepare(" INSERT INTO CHAT_BANNED_USERS "
                      " (chat_id, user_id) "
                      " VALUES (:chat_id, :user_id); ");
    } else {
        query.prepare(" DELETE FROM CHAT_BANNED_USERS "
                      " WHERE chat_id=:chat_id AND user_id=:user_id; ");
    }

    query.bindValue(":chat_id", chat_id);
    query.bindValue(":user_id", user_id);

    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString() << '\n';
        return false;
    } else {
        // если блокировка была успешна, то row affected > 0, иначе row affected == 0 (false).
        bool is_quit = true;
        if (is_ban) {
            is_quit = quit_chat(user_id, chat_id);
        }

        return query.numRowsAffected() && is_quit; // блокировка и удаление пользователя из чата
    }
}

bool LinkDoveSQLDataBase::is_banned_ind_user(unsigned long long from_id, unsigned long long to_id) {
    QSqlQuery query(data_base_);

    query.prepare(" SELECT * FROM INTERLOCUTOR_BANNED_USERS "
                  " WHERE from_id=:from_id AND to_id=:to_id; ");

    query.bindValue(":from_id", from_id);
    query.bindValue(":to_id", to_id);

    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString() << '\n';
        throw std::runtime_error("is_banned_ind_user failed due to exec.");
    } else {
        if (query.next()) {
            return true;
        } else {
            return false;
        }
    }
}

bool LinkDoveSQLDataBase::is_banned_chat_user(unsigned long long chat_id, unsigned long long user_id) {
    QSqlQuery query(data_base_);

    query.prepare(" SELECT * FROM CHAT_BANNED_USERS "
                  " WHERE chat_id=:chat_id AND user_id=:user_id; ");

    query.bindValue(":chat_id", chat_id);
    query.bindValue(":user_id", user_id);

    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString() << '\n';
        throw std::runtime_error("is_banned_chat_user failed due to exec.");
    } else {
        if (query.next()) {
            return true;
        } else {
            return false;
        }
    }
}

bool LinkDoveSQLDataBase::get_user_banned_status(const std::string &username) {
    QSqlQuery query(data_base_);
    query.prepare(" SELECT * FROM USERS "
                  " WHERE username=:username; ");

    query.bindValue(":username", username.c_str());

    if (!query.exec() || !query.next()) {
        std::cerr << query.lastError().text().toStdString() << '\n';
        throw std::runtime_error("Failed to check user banned status");
    } else {
        return query.value("is_banned").toBool();
    }
}

std::vector<std::string> LinkDoveSQLDataBase::get_banned_interlocutors(unsigned long long id) {
    QSqlQuery query(data_base_);
    query.prepare(" SELECT * FROM INTERLOCUTOR_BANNED_USERS "
                  " WHERE from_id=:id; ");

    query.bindValue(":id", id);

    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString() << '\n';
        throw std::runtime_error("get_banned_interlocutors failed due to query.exec");
    }

    std::vector<std::string> interlocutors;
    while (query.next()) {
        interlocutors.push_back(get_status_info(query.value("to_id").toULongLong()).username_);
    }

    return interlocutors;
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
    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString() << '\n';
        return false;
    } else {
        // если удаление было успешным, то row affected > 0, иначе row affected == 0 (false).
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
        throw std::runtime_error("get_complaints failed due to query.exec");
    } else {
        if (!query.next()) {
            return std::vector<Complaint>(); // в БД нет жалоб
        } else {
            return link_dove_database_details__::retrieve_complaints(query, count);
        }
    }
}

bool LinkDoveSQLDataBase::add_answer(unsigned long long user_id, const std::string &text) {
    QSqlQuery query(data_base_);

    query.prepare(" INSERT INTO ANSWERS "
                  " (receiver_id, text) "
                  " VALUES (:user_id, :text); ");

    query.bindValue(":user_id", user_id);
    query.bindValue(":text", text.c_str());

    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString() << '\n';
        return false;
    } else {
        // если вставка была успешна, то row affected > 0, иначе row affected == 0 (false).
        return query.numRowsAffected();
    }
}

std::vector<Notification> LinkDoveSQLDataBase::get_notifications(unsigned long long user_id, int count) {
    QSqlQuery query(data_base_);
    query.prepare(" SELECT * FROM ANSWERS "
                  " WHERE receiver_id=:user_id; ");

    query.bindValue(":user_id", user_id);

    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString() << '\n';
        throw std::runtime_error("get_notifications failed due to query.exec");
    } else {
        if (!query.next()) {
            return std::vector<Notification>(); // в БД нет уведомлений
        } else {
            return link_dove_database_details__::retrieve_notifications(query, count);
        }
    }
}

bool LinkDoveSQLDataBase::delete_notification(unsigned long long id) {
    QSqlQuery query(data_base_);

    query.prepare(" DELETE FROM ANSWERS "
                  " where ID=:id; ");

    query.bindValue(":id", id);

    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString() << '\n';
        return false;
    } else {
        // если удаление было успешно, то row affected > 0, иначе row affected == 0 (false).
        return query.numRowsAffected();
    }
}

bool LinkDoveSQLDataBase::add_ind_message(const IMessage& msg) {
    QSqlQuery query(data_base_);
    unsigned long long msg_id = 0;

    // используем мьютекс для того, чтобы во время транзакции не было модификации данных
    std::unique_lock<std::mutex> unique_mtx(modify_mutex);

    // используем транзакцию, чтобы в случае ошибки добавления сообщения данные в БД были
    // в согласованном состоянии
    if (data_base_.transaction()) {
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
                return false;
            } else {
                msg_id = query.value("ID").toULongLong();
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
            case IMAGE_MSG_TYPE: {
                query.prepare(" INSERT INTO IND_IMAGE_MESSAGE_CONTENTS "
                              " (msg_id, image_path) "
                              " VALUES (:msg_id, :image_path); ");
                query.bindValue(":msg_id", msg_id);
                query.bindValue(":image_path", msg.get_msg_content()->get_raw_data());

                // ПОВТОР КОДА!!!
                if (!query.exec()) {
                    std::cerr << query.lastError().text().toStdString() << '\n';
                    return false;
                }

                query.prepare(" SELECT * FROM IND_IMAGE_MESSAGE_CONTENTS "
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

                content_enum = "image";
                break;
            }

            case AUDIO_MSG_TYPE: {
                query.prepare(" INSERT INTO IND_AUDIO_MESSAGE_CONTENTS "
                              " (msg_id, audio_path) "
                              " VALUES (:msg_id, :audio_path); ");
                query.bindValue(":msg_id", msg_id);
                query.bindValue(":audio_path", msg.get_msg_content()->get_raw_data());

                // ПОВТОР КОДА!!!
                if (!query.exec()) {
                    std::cerr << query.lastError().text().toStdString() << '\n';
                    return false;
                }

                query.prepare(" SELECT * FROM IND_AUDIO_MESSAGE_CONTENTS "
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

                content_enum = "audio";
                break;
            }
        }

        query.prepare(" UPDATE INDIVIDUAL_MESSAGES "
                      " SET content_id   = :content_id,"
                      "     content_type = :content_type "
                      " WHERE ID = :msg_id; ");

        query.bindValue(":content_id", content_id);
        query.bindValue(":content_type", content_enum.c_str());
        query.bindValue(":msg_id", msg_id);

        if (!query.exec()) {
            std::cerr << query.lastError().text().toStdString() << '\n';
            return false;
        }

        if (!data_base_.commit()) {
            std::cerr << "Failed to commit adding new individual message.\n";
            return false;
        }
    } else {
        std::cerr << "Failed to start transaction during adding new individual message.\n";
        return false;
    }

    query.exec(" COMMIT; ");
    return true;
}

bool LinkDoveSQLDataBase::delete_ind_message(const IMessage &msg) {
    QSqlQuery query(data_base_);

    query.prepare(" DELETE FROM INDIVIDUAL_MESSAGES "
                  " WHERE ID=:id; ");

    query.bindValue(":id", msg.get_id());
    std::cerr << msg.get_id() << '\n';

    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString() << '\n';
        return false;
    } else {
        // если удаление было успешно, то row affected > 0, иначе row affected == 0 (false).
        return query.numRowsAffected();
    }
}

bool LinkDoveSQLDataBase::add_chnnl_message(const IMessage& msg) {
    QSqlQuery query(data_base_);
    unsigned long long msg_id = 0;

    // используем мьютекс для того, чтобы во время транзакции не было модификации данных
    std::unique_lock<std::mutex> unique_mtx(modify_mutex);

    // используем транзакцию, чтобы в случае ошибки добавления сообщения данные в БД были
    // в согласованном состоянии
    if (data_base_.transaction()) {
        query.prepare(" INSERT INTO CHANNEL_MESSAGES "
                      " (channel_id, send_datetime) "
                      " VALUES (:channel_id, NOW()); ");

        query.bindValue(":channel_id", static_cast<const ChannelMessage&>(msg).get_channel_id());

        if (!query.exec()) {
            std::cerr << query.lastError().text().toStdString() << '\n';
            return false;
        }

        query.prepare("SELECT * FROM CHANNEL_MESSAGES "
                      "WHERE "
                      " channel_id=:channel_id AND content_id=0; ");
        query.bindValue(":channel_id", static_cast<const ChannelMessage&>(msg).get_channel_id());

        if (!query.exec()) {
            std::cerr << query.lastError().text().toStdString() << '\n';
            return false;
        } else {
            if (!query.next()) {
                return false;
            } else {
                msg_id = query.value("ID").toULongLong();
            }
        }


        std::string content_enum;
        unsigned long long content_id = 0;
        switch (msg.get_msg_content()->get_msg_content_type()) {
            case TEXT_MSG_TYPE: {
                query.prepare(" INSERT INTO CHANNEL_TEXT_MESSAGE_CONTENTS "
                              " (msg_id, text_data) "
                              " VALUES (:msg_id, :text_data); ");
                query.bindValue(":msg_id", msg_id);
                query.bindValue(":text_data", msg.get_msg_content()->get_raw_data());

                if (!query.exec()) {
                    std::cerr << query.lastError().text().toStdString() << '\n';
                    return false;
                }

                query.prepare(" SELECT * FROM CHANNEL_TEXT_MESSAGE_CONTENTS "
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
            case IMAGE_MSG_TYPE: {
                query.prepare(" INSERT INTO CHANNEL_IMAGE_MESSAGE_CONTENTS "
                              " (msg_id, image_path) "
                              " VALUES (:msg_id, :image_path); ");
                query.bindValue(":msg_id", msg_id);
                query.bindValue(":image_path", msg.get_msg_content()->get_raw_data());

                // ПОВТОР КОДА!!!
                if (!query.exec()) {
                    std::cerr << query.lastError().text().toStdString() << '\n';
                    return false;
                }

                query.prepare(" SELECT * FROM CHANNEL_IMAGE_MESSAGE_CONTENTS "
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

                content_enum = "image";
                break;
            }
            case AUDIO_MSG_TYPE: {
                query.prepare(" INSERT INTO CHANNEL_AUDIO_MESSAGE_CONTENTS "
                              " (msg_id, audio_path) "
                              " VALUES (:msg_id, :audio_path); ");
                query.bindValue(":msg_id", msg_id);
                query.bindValue(":audio_path", msg.get_msg_content()->get_raw_data());

                // ПОВТОР КОДА!!!
                if (!query.exec()) {
                    std::cerr << query.lastError().text().toStdString() << '\n';
                    return false;
                }

                query.prepare(" SELECT * FROM CHANNEL_AUDIO_MESSAGE_CONTENTS "
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

                content_enum = "audio";
                break;
            }
        }

        query.prepare(" UPDATE CHANNEL_MESSAGES "
                      " SET content_id   = :content_id,"
                      "     content_type = :content_type "
                      " WHERE ID = :msg_id; ");

        query.bindValue(":content_id", content_id);
        query.bindValue(":content_type", content_enum.c_str());
        query.bindValue(":msg_id", msg_id);

        if (!query.exec()) {
            std::cerr << query.lastError().text().toStdString() << '\n';
            return false;
        }

        if (!data_base_.commit()) {
            std::cerr << "Failed to commit adding new channel message.\n";
            return false;
        }
    } else {
        std::cerr << "Failed to start transaction during adding new channel message.\n";
        return false;
    }

    query.exec(" COMMIT; ");
    return true;
}

bool LinkDoveSQLDataBase::delete_chnnl_message(const IMessage& msg) {
    QSqlQuery query(data_base_);

    query.prepare(" DELETE FROM CHANNEL_MESSAGES "
                  " WHERE ID=:id; ");

    query.bindValue(":id", msg.get_id());

    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString() << '\n';
        return false;
    } else {
        // если удаление было успешно, то row affected > 0, иначе row affected == 0 (false).
        return query.numRowsAffected();
    }
}

std::vector<std::shared_ptr<IMessage>> LinkDoveSQLDataBase::get_ind_messages(unsigned long long sender_id, unsigned long long receiver_id) {
    QSqlQuery query(data_base_);

    query.prepare("SELECT * FROM INDIVIDUAL_MESSAGES "
                  " WHERE sender_id=:sender_id and receiver_id=:receiver_id; ");

    query.bindValue(":sender_id", sender_id);
    query.bindValue(":receiver_id", receiver_id);

    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString() << '\n';
        throw std::runtime_error("get_ind_messages failed due to query.exec\n");
    }

    if (!query.next()) {
        return std::vector<std::shared_ptr<IMessage>>(); // в БД нет сообщений с указанными требованиями
    } else {
        QSqlQuery content_query(data_base_);
        return link_dove_database_details__::retrieve_ind_messages(query, content_query);
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
        throw std::runtime_error("get_interlocutors failed due to query.exec");
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
        throw std::runtime_error("get_interlocutors failed due to query.exec");
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

bool LinkDoveSQLDataBase::delete_ind_chat(unsigned long long first_id, unsigned long long second_id) {
    QSqlQuery query(data_base_);
    query.prepare(" DELETE FROM INDIVIDUAL_MESSAGES "
                  " WHERE "
                  " sender_id=:first_id AND receiver_id=:second_id "
                  " OR "
                  " sender_id=:second_id AND receiver_id=:first_id; ");

    query.bindValue(":first_id", first_id);
    query.bindValue(":second_id", second_id);

    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString() << '\n';
        return false;
    } else {
        // если удаление было успешным, то row affected > 0, иначе row affected == 0 (false).
        return query.numRowsAffected();
    }
}

bool LinkDoveSQLDataBase::add_channel(const ChannelInfo &channel_info) {
    QSqlQuery query(data_base_);
    query.prepare(" INSERT INTO CHANNELS "
                  " (owner_id, name, is_private) "
                  " VALUES (:owner_id, :name, :is_private); ");

    query.bindValue(":owner_id", channel_info.owner_id_);
    query.bindValue(":name", channel_info.name_.c_str());
    query.bindValue(":is_private", channel_info.is_private_);

    if (data_base_.transaction()) {
        if (!query.exec()) {
            std::cerr << query.lastError().text().toStdString();
            return false;
        } else {
            // если вставка была успешна, то row affected > 0, иначе row affected == 0 (false).
            if (query.numRowsAffected()) {
                try {
                    ChannelInfo updated_channel_info = get_channel(channel_info.name_);
                    if (add_participant_to_channel(updated_channel_info.owner_id_, updated_channel_info.id_)) {
                        if (!data_base_.commit()) {
                            std::cerr << "Failed to commit adding new channel.\n";
                            return false;
                        } else {
                            return true;
                        }
                    } else {
                        return false;
                    }
                } catch (std::runtime_error &ex) {
                    std::cerr << ex.what() << '\n';
                    return false;
                }
            } else {
                return false;
            }
        }
    } else {
        std::cerr << "Failed to start transaction during adding new channel\n";
        return false;
    }
}

std::vector<ChannelInfo> LinkDoveSQLDataBase::get_channels(unsigned long long id) {
    QSqlQuery query(data_base_);
    query.prepare(" SELECT * FROM CHANNEL_PARTICIPANTS "
                  " WHERE participant_id=:id; ");

    query.bindValue(":id", id);

    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString() << '\n';
        throw std::runtime_error("get_channels failed due to query.exec");
    }

    std::vector<ChannelInfo> channels;
    while (query.next()) {
        channels.push_back(get_channel(query.value("channel_id").toULongLong()));
    }

    return channels;
}

ChannelInfo LinkDoveSQLDataBase::get_channel(const std::string &channel_name) {
    QSqlQuery query(data_base_);
    query.prepare(" SELECT * FROM CHANNELS "
                  " WHERE name=:name; ");

    query.bindValue(":name", channel_name.c_str());
    if (!query.exec()) {
        throw std::runtime_error(query.lastError().text().toStdString());
    } else {
        if (!query.next()) {
            throw std::runtime_error("get_channel failed: no such channel in DataBase");
        } else {
            return link_dove_database_details__::retrieve_channel_info(query);
        }
    }
}

ChannelInfo LinkDoveSQLDataBase::get_channel(unsigned long long channel_id) {
    QSqlQuery query(data_base_);
    query.prepare(" SELECT * FROM CHANNELS "
                  " WHERE ID=:id; ");

    query.bindValue(":id", channel_id);
    if (!query.exec()) {
        throw std::runtime_error(query.lastError().text().toStdString());
    } else {
        if (!query.next()) {
            throw std::runtime_error("get_channel failed: no such channel in DataBase");
        } else {
            return link_dove_database_details__::retrieve_channel_info(query);
        }
    }
}

bool LinkDoveSQLDataBase::add_participant_to_channel(unsigned long long participant_id, unsigned long long channel_id) {
    QSqlQuery query(data_base_);
    query.prepare(" INSERT INTO CHANNEL_PARTICIPANTS "
                  " (channel_id, participant_id) "
                  " VALUES (:channel_id, :participant_id); ");

    query.bindValue(":channel_id", channel_id);
    query.bindValue(":participant_id", participant_id);

    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString();
        return false;
    } else {
        // если вставка была успешна, то row affected > 0, иначе row affected == 0 (false).
        return query.numRowsAffected();
    }
}

bool LinkDoveSQLDataBase::request_participant_to_channel(unsigned long long user_id, unsigned long long channel_id) {
    QSqlQuery query(data_base_);
    query.prepare(" INSERT INTO CHANNEL_REQUESTS "
                  " (channel_id, user_id) "
                  " VALUES (:channel_id, :user_id); ");

    query.bindValue(":channel_id", channel_id);
    query.bindValue(":user_id", user_id);

    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString();
        return false;
    } else {
        // если вставка была успешна, то row affected > 0, иначе row affected == 0 (false).
        return query.numRowsAffected();
    }
}

bool LinkDoveSQLDataBase::remove_request_channel(unsigned long long user_id, unsigned long long channel_id) {
    QSqlQuery query(data_base_);

    query.prepare(" DELETE FROM CHANNEL_REQUESTS "
                  " WHERE user_id=:user_id AND channel_id=:channel_id;");

    query.bindValue(":channel_id", channel_id);
    query.bindValue(":user_id", user_id);

    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString();
        return false;
    } else {
        // если удаление было успешно, то row affected > 0, иначе row affected == 0 (false).
        return query.numRowsAffected();
    }
}

bool LinkDoveSQLDataBase::request_participant_to_chat(unsigned long long user_id, unsigned long long chat_id) {
    QSqlQuery query(data_base_);
    query.prepare(" INSERT INTO CHAT_REQUESTS "
                  " (chat_id, user_id) "
                  " VALUES (:chat_id, :user_id); ");

    query.bindValue(":chat_id", chat_id);
    query.bindValue(":user_id", user_id);

    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString();
        return false;
    } else {
        // если вставка была успешна, то row affected > 0, иначе row affected == 0 (false).
        return query.numRowsAffected();
    }
}

bool LinkDoveSQLDataBase::remove_request_chat(unsigned long long user_id, unsigned long long chat_id) {
    QSqlQuery query(data_base_);

    query.prepare(" DELETE FROM CHAT_REQUESTS "
                  " WHERE user_id=:user_id AND chat_id=:chat_id;");

    query.bindValue(":chat_id", chat_id);
    query.bindValue(":user_id", user_id);

    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString();
        return false;
    } else {
        // если удаление было успешно, то row affected > 0, иначе row affected == 0 (false).
        return query.numRowsAffected();
    }
}

bool LinkDoveSQLDataBase::is_channel_participant(unsigned long long participant_id, unsigned long long channel_id) {
    QSqlQuery query(data_base_);
    query.prepare(" SELECT * FROM CHANNEL_PARTICIPANTS "
                  " WHERE "
                  " channel_id=:channel_id AND participant_id=:participant_id; ");

    query.bindValue(":channel_id", channel_id);
    query.bindValue(":participant_id", participant_id);

    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString();
        throw std::runtime_error("is_channel_participant failed due to exec.");
    } else {
        if (query.next()) {
            return true;
        } else {
            return false;
        }
    }
}

std::vector<std::shared_ptr<IMessage>> LinkDoveSQLDataBase::get_channel_messages(unsigned long long channel_id) {
    QSqlQuery query(data_base_);

    query.prepare("SELECT * FROM CHANNEL_MESSAGES "
                  " WHERE channel_id=:channel_id; ");

    query.bindValue(":channel_id", channel_id);

    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString() << '\n';
        throw std::runtime_error("get_channel_messages failed due to query.exec");
    }

    if (!query.next()) {
        return std::vector<std::shared_ptr<IMessage>>(); // в БД нет сообщений с указанными требованиями
    } else {
        QSqlQuery content_query(data_base_);
        return link_dove_database_details__::retrieve_channel_messages(query, content_query);
    }
}

std::vector<std::string> LinkDoveSQLDataBase::get_channel_participants(unsigned long long channel_id) {
    QSqlQuery query(data_base_);

    query.prepare(" SELECT participant_id FROM CHANNEL_PARTICIPANTS "
                  " WHERE channel_id=:channel_id; ");

    query.bindValue(":channel_id", channel_id);

    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString() << '\n';
        throw std::runtime_error("get_channel_requests failed due to query.exec");
    }

    std::vector<std::string> participants;
    while (query.next()) {
        participants.push_back(get_status_info(query.value("participant_id").toULongLong()).username_);
    }

    return participants;
}

std::vector<std::string> LinkDoveSQLDataBase::get_channel_requests(unsigned long long channel_id) {
    QSqlQuery query(data_base_);

    query.prepare(" SELECT user_id FROM CHANNEL_REQUESTS "
                  " WHERE channel_id=:channel_id; ");

    query.bindValue(":channel_id", channel_id);

    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString() << '\n';
        throw std::runtime_error("get_channel_requests failed due to query.exec");
    }

    std::vector<std::string> participants;
    while (query.next()) {
        participants.push_back(get_status_info(query.value("user_id").toULongLong()).username_);
    }

    return participants;
}

std::vector<std::string> LinkDoveSQLDataBase::get_chat_requests(unsigned long long chat_id) {
    QSqlQuery query(data_base_);

    query.prepare(" SELECT user_id FROM CHAT_REQUESTS "
                  " WHERE chat_id=:chat_id; ");

    query.bindValue(":chat_id", chat_id);

    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString() << '\n';
        throw std::runtime_error("get_channel_requests failed due to query.exec");
    }

    std::vector<std::string> participants;
    while (query.next()) {
        participants.push_back(get_status_info(query.value("user_id").toULongLong()).username_);
    }

    return participants;
}

bool LinkDoveSQLDataBase::delete_channel(unsigned long long channel_id) {
    QSqlQuery query(data_base_);

    // Удаляем все текстовые сообщения
    query.prepare(" DELETE FROM CHANNELS "
                  " WHERE ID=:id; ");

    query.bindValue(":id", channel_id);

    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString() << '\n';
        return false;
    } else {
        // если удаление было успешным, то row affected > 0, иначе row affected == 0 (false).
        return query.numRowsAffected();
    }
}

bool LinkDoveSQLDataBase::quit_channel(unsigned long long user_id, unsigned long long channel_id) {
    QSqlQuery query(data_base_);
    query.prepare(" DELETE FROM CHANNEL_PARTICIPANTS "
                  " WHERE participant_id=:participant_id AND channel_id=:channel_id; " );

    query.bindValue(":participant_id", user_id);
    query.bindValue(":channel_id", channel_id);

    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString() << '\n';
        return false;
    } else {
        // если удаление было успешным, то row affected > 0, иначе row affected == 0 (false).
        return query.numRowsAffected();
    }
}

bool LinkDoveSQLDataBase::add_chat(const ChatInfo &chat_info) {
    QSqlQuery query(data_base_);
    query.prepare(" INSERT INTO CHATS "
                  " (owner_id, name, is_private) "
                  " VALUES (:owner_id, :name, :private); ");

    query.bindValue(":owner_id", chat_info.owner_id_);
    query.bindValue(":name", chat_info.name_.c_str());
    query.bindValue(":private", chat_info.is_private_);

    if (data_base_.transaction()) {
        if (!query.exec()) {
            std::cerr << query.lastError().text().toStdString() << '\n';
            return false;
        } else {
            // если вставка была успешна, то row affected > 0, иначе row affected == 0 (false).
            if (query.numRowsAffected()) {
                try {
                    ChatInfo updated_chat_info = get_chat(chat_info.name_);
                    if (add_participant_to_chat(updated_chat_info.owner_id_, updated_chat_info.id_)) {
                        if (!data_base_.commit()) {
                            std::cerr << "Failed to commit adding new chat.\n";
                            return false;
                        } else {
                            return true;
                        }
                    } else {
                        return false;
                    }
                } catch (std::runtime_error &ex) {
                    std::cerr << ex.what() << '\n';
                    return false;
                }
            } else {
                return false;
            }
        }
    } else {
        std::cerr << "Failed to start transaction during adding new chat.\n";
        return false;
    }
}

ChatInfo LinkDoveSQLDataBase::get_chat(const std::string &chat_name) {
    QSqlQuery query(data_base_);
    query.prepare(" SELECT * FROM CHATS "
                  " WHERE name=:name; ");

    query.bindValue(":name", chat_name.c_str());
    if (!query.exec()) {
        throw std::runtime_error(query.lastError().text().toStdString());
    } else {
        if (!query.next()) {
            throw std::runtime_error("get_chat failed: no such chat in DataBase");
        } else {
            return link_dove_database_details__::retrieve_chat_info(query);
        }
    }
}

ChatInfo LinkDoveSQLDataBase::get_chat(unsigned long long id) {
    QSqlQuery query(data_base_);
    query.prepare(" SELECT * FROM CHATS "
                  " WHERE ID=:id; ");

    query.bindValue(":id", id);
    if (!query.exec()) {
        throw std::runtime_error(query.lastError().text().toStdString());
    } else {
        if (!query.next()) {
            throw std::runtime_error("get_chat failed: no such chat in DataBase");
        } else {
            return link_dove_database_details__::retrieve_chat_info(query);
        }
    }
}

bool LinkDoveSQLDataBase::add_participant_to_chat(unsigned long long participant_id, unsigned long long chat_id) {
    QSqlQuery query(data_base_);
    query.prepare(" INSERT INTO CHAT_PARTICIPANTS "
                  " (chat_id, participant_id) "
                  " VALUES (:chat_id, :participant_id); ");

    query.bindValue(":chat_id", chat_id);
    query.bindValue(":participant_id", participant_id);

    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString();
        return false;
    } else {
        // если вставка была успешна, то row affected > 0, иначе row affected == 0 (false).
        return query.numRowsAffected();
    }
}

bool LinkDoveSQLDataBase::is_chat_participant(unsigned long long participant_id, unsigned long long chat_id) {
    QSqlQuery query(data_base_);
    query.prepare(" SELECT * FROM CHAT_PARTICIPANTS "
                  " WHERE "
                  " chat_id=:chat_id AND participant_id=:participant_id; ");

    query.bindValue(":chat_id", chat_id);
    query.bindValue(":participant_id", participant_id);

    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString();
        throw std::runtime_error("is_chat_participant failed due to query.exec");
    } else {
        if (query.next()) {
            return true;
        } else {
            return false;
        }
    }
}

std::vector<ChatInfo> LinkDoveSQLDataBase::get_chats(unsigned long long id) {
    QSqlQuery query(data_base_);
    query.prepare(" SELECT * FROM CHAT_PARTICIPANTS "
                  " WHERE participant_id=:id; ");

    query.bindValue(":id", id);

    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString() << '\n';
        throw std::runtime_error("get_chats failed due to query.exec");
    }

    std::vector<ChatInfo> chats;
    while (query.next()) {
        chats.push_back(get_chat(query.value("chat_id").toULongLong()));
    }

    return chats;
}

bool LinkDoveSQLDataBase::delete_chat(unsigned long long chat_id) {
    QSqlQuery query(data_base_);

    // Удаляем все текстовые сообщения
    query.prepare(" DELETE FROM CHATS "
                  " WHERE ID=:id; ");

    query.bindValue(":id", chat_id);
    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString() << '\n';
        return false;
    } else {
        // если удаление было успешным, то row affected > 0, иначе row affected == 0 (false).
        return query.numRowsAffected();
    }
}

bool LinkDoveSQLDataBase::quit_chat(unsigned long long user_id, unsigned long long chat_id) {
    QSqlQuery query(data_base_);
    query.prepare(" DELETE FROM CHAT_PARTICIPANTS "
                  " WHERE participant_id=:participant_id AND chat_id=:chat_id; " );

    query.bindValue(":participant_id", user_id);
    query.bindValue(":chat_id", chat_id);

    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString() << '\n';
        return false;
    } else {
        // если удаление было успешным, то row affected > 0, иначе row affected == 0 (false).
        return query.numRowsAffected();
    }
}

bool LinkDoveSQLDataBase::add_chat_message(const IMessage& msg) {
    QSqlQuery query(data_base_);
    unsigned long long msg_id = 0;

    // используем мьютекс для того, чтобы во время транзакции не было модификации данных
    std::unique_lock<std::mutex> unique_mtx(modify_mutex);

    // используем транзакцию, чтобы в случае ошибки добавления сообщения данные в БД были
    // в согласованном состоянии
    if (data_base_.transaction()) {
        query.prepare(" INSERT INTO CHAT_MESSAGES "
                      " (chat_id, send_datetime, owner_id) "
                      " VALUES (:chat_id, NOW(), :owner_id); ");

        query.bindValue(":chat_id", static_cast<const GroupMessage&>(msg).get_group_id());
        query.bindValue(":owner_id", static_cast<const GroupMessage&>(msg).get_owner_id());

        if (!query.exec()) {
            std::cerr << query.lastError().text().toStdString() << '\n';
            return false;
        }

        query.prepare("SELECT * FROM CHAT_MESSAGES "
                      "WHERE "
                      " chat_id=:chat_id AND content_id=0; ");
        query.bindValue(":chat_id", static_cast<const GroupMessage&>(msg).get_group_id());

        if (!query.exec()) {
            std::cerr << query.lastError().text().toStdString() << '\n';
            return false;
        } else {
            if (!query.next()) {
                std::cerr << "Failed to add chat message due to incorrect inserting in CHAT_MESSAGES.";
                return false;
            } else {
                msg_id = query.value("ID").toULongLong();
            }
        }


        std::string content_enum;
        unsigned long long content_id = 0;
        switch (msg.get_msg_content()->get_msg_content_type()) {
            case TEXT_MSG_TYPE: {
                query.prepare(" INSERT INTO CHAT_TEXT_MESSAGE_CONTENTS "
                              " (msg_id, text_data) "
                              " VALUES (:msg_id, :text_data); ");
                query.bindValue(":msg_id", msg_id);
                query.bindValue(":text_data", msg.get_msg_content()->get_raw_data());

                if (!query.exec()) {
                    std::cerr << query.lastError().text().toStdString() << '\n';
                    return false;
                }

                query.prepare(" SELECT * FROM CHAT_TEXT_MESSAGE_CONTENTS "
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
            case IMAGE_MSG_TYPE: {
                query.prepare(" INSERT INTO CHAT_IMAGE_MESSAGE_CONTENTS "
                              " (msg_id, image_path) "
                              " VALUES (:msg_id, :image_path); ");
                query.bindValue(":msg_id", msg_id);
                query.bindValue(":image_path", msg.get_msg_content()->get_raw_data());

                // ПОВТОР КОДА!!!
                if (!query.exec()) {
                    std::cerr << query.lastError().text().toStdString() << '\n';
                    return false;
                }

                query.prepare(" SELECT * FROM CHAT_IMAGE_MESSAGE_CONTENTS "
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

                content_enum = "image";
                break;
            }
            case AUDIO_MSG_TYPE: {
                query.prepare(" INSERT INTO CHAT_AUDIO_MESSAGE_CONTENTS "
                              " (msg_id, audio_path) "
                              " VALUES (:msg_id, :audio_path); ");
                query.bindValue(":msg_id", msg_id);
                query.bindValue(":audio_path", msg.get_msg_content()->get_raw_data());

                // ПОВТОР КОДА!!!
                if (!query.exec()) {
                    std::cerr << query.lastError().text().toStdString() << '\n';
                    return false;
                }

                query.prepare(" SELECT * FROM CHAT_AUDIO_MESSAGE_CONTENTS "
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

                content_enum = "audio";
                break;
            }
        }

        query.prepare(" UPDATE CHAT_MESSAGES "
                      " SET content_id   = :content_id,"
                      "     content_type = :content_type "
                      " WHERE ID = :msg_id; ");

        query.bindValue(":content_id", content_id);
        query.bindValue(":content_type", content_enum.c_str());
        query.bindValue(":msg_id", msg_id);

        if (!query.exec()) {
            std::cerr << query.lastError().text().toStdString() << '\n';
            return false;
        }

        if (!data_base_.commit()) {
            std::cerr << "Failed to commit adding new chat message.\n";
            return false;
        }
    } else {
        std::cerr << "Failed to start transaction during adding new chat message.\n";
        return false;
    }

    query.exec(" COMMIT; ");
    return true;
}

bool LinkDoveSQLDataBase::delete_chat_message(const IMessage& msg) {
    QSqlQuery query(data_base_);

    query.prepare(" DELETE FROM CHAT_MESSAGES "
                  " WHERE ID=:id; ");

    query.bindValue(":id", msg.get_id());

    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString() << '\n';
        return false;
    } else {
        // если удаление было успешно, то row affected > 0, иначе row affected == 0 (false).
        return query.numRowsAffected();
    }
}

std::vector<std::shared_ptr<IMessage>> LinkDoveSQLDataBase::get_chat_messages(unsigned long long chat_id) {
    QSqlQuery query(data_base_);

    query.prepare(" SELECT * FROM CHAT_MESSAGES "
                  " WHERE chat_id=:chat_id; ");

    query.bindValue(":chat_id", chat_id);

    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString() << '\n';
        throw std::runtime_error("get_channel_messages failed: cannot get channel messages\n");
    }

    if (!query.next()) {
        return std::vector<std::shared_ptr<IMessage>>(); // в БД нет сообщений с указанными требованиями
    } else {
        QSqlQuery content_query(data_base_);
        StatusInfo user_info;

        std::vector<std::shared_ptr<IMessage>> messages;
        std::string msg_type;
        unsigned long long chat_id = query.value("chat_id").toULongLong();

        do {
            user_info = get_status_info(query.value("owner_id").toULongLong());

            std::shared_ptr<GroupMessage> message_ptr = std::make_shared<GroupMessage>();
            message_ptr->set_id(query.value("ID").toULongLong());
            message_ptr->set_send_datetime(query.value("send_datetime").toDateTime());
            message_ptr->set_group_id(chat_id);
            message_ptr->set_owner_id(query.value("owner_id").toULongLong());
            message_ptr->set_owner_name(user_info.username_);

            msg_type = query.value("content_type").toString().toStdString();
            if (msg_type == "text") {
                content_query.prepare("SELECT * FROM CHAT_TEXT_MESSAGE_CONTENTS "
                                      "WHERE msg_id=:msg_id; ");

                content_query.bindValue(":msg_id", message_ptr->get_id());

                if (!content_query.exec() || !content_query.next()) {
                    std::cerr << content_query.lastError().text().toStdString() << '\n';
                    continue;
                }

                std::shared_ptr<TextMessageContent> text_msg_content_ptr = std::make_shared<TextMessageContent>();
                text_msg_content_ptr->set_text(content_query.value("text_data").toString().toStdString());
                message_ptr->set_msg_content(text_msg_content_ptr);
            //  ПОВТОР КОДА! УБРАТЬ!
            } else if (msg_type == "audio") {
                content_query.prepare("SELECT * FROM CHAT_AUDIO_MESSAGE_CONTENTS "
                                      "WHERE msg_id=:msg_id; ");
                content_query.bindValue(":msg_id", message_ptr->get_id());

                if (!content_query.exec() || !content_query.next()) {
                    std::cerr << content_query.lastError().text().toStdString() << '\n';
                    continue;
                }

                std::shared_ptr<AudioMessageContent> text_msg_content_ptr = std::make_shared<AudioMessageContent>();
                text_msg_content_ptr->set_audio_path(content_query.value("audio_path").toString().toStdString());
                message_ptr->set_msg_content(text_msg_content_ptr);
            } else if (msg_type == "image") {
                content_query.prepare("SELECT * FROM CHAT_IMAGE_MESSAGE_CONTENTS "
                                      "WHERE msg_id=:msg_id; ");
                content_query.bindValue(":msg_id", message_ptr->get_id());

                if (!content_query.exec() || !content_query.next()) {
                    std::cerr << content_query.lastError().text().toStdString() << '\n';
                    continue;
                }

                std::shared_ptr<ImageMessageContent> text_msg_content_ptr = std::make_shared<ImageMessageContent>();
                text_msg_content_ptr->set_image_path(content_query.value("image_path").toString().toStdString());
                message_ptr->set_msg_content(text_msg_content_ptr);
            }

            messages.push_back(message_ptr);
        } while (query.next());

        return messages;
    }
}

std::vector<std::string> LinkDoveSQLDataBase::get_chat_participants(unsigned long long group_id) {
    QSqlQuery query(data_base_);

    query.prepare(" SELECT participant_id FROM CHAT_PARTICIPANTS "
                  " WHERE chat_id=:chat_id; ");

    query.bindValue(":chat_id", group_id);

    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString() << '\n';
        throw std::runtime_error("get_chat_participants failed due to query.exec");
    }

    std::vector<std::string> participants;
    while (query.next()) {
        participants.push_back(get_status_info(query.value("participant_id").toULongLong()).username_);
    }

    return participants;
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
            throw std::runtime_error("get_status_info failed: no such user in DataBase");
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
            throw std::runtime_error("get_status_info failed: no such user in DataBase");
        } else {
            return link_dove_database_details__::retrieve_status_info(query);
        }
    }
}

namespace link_dove_database_details__ {
    ChannelInfo retrieve_channel_info(const QSqlQuery& query) {
        ChannelInfo channel_info;
        channel_info.id_        = query.value("ID").toULongLong();
        channel_info.owner_id_  = query.value("owner_id").toULongLong();
        channel_info.name_      = query.value("name").toString().toStdString();
        channel_info.is_private_ = query.value("is_private").toBool();

        return channel_info;
    }

    ChatInfo retrieve_chat_info(const QSqlQuery &query) {
        ChatInfo chat_info;
        chat_info.id_        = query.value("ID").toULongLong();
        chat_info.owner_id_  = query.value("owner_id").toULongLong();
        chat_info.name_      = query.value("name").toString().toStdString();
        chat_info.is_private_ = query.value("is_private").toBool();

        return chat_info;
    }

    StatusInfo retrieve_status_info(const QSqlQuery& query) {
        StatusInfo status_info;
        status_info.id_          = query.value("ID").toULongLong();
        status_info.username_    = query.value("username").toString().toStdString();
        status_info.email_       = query.value("email").toString().toStdString();
        status_info.birthday_    = QDate::fromString(query.value("birthday").toString(), QString(BIRTHAY_FORMAT));
        status_info.text_status_ = query.value("text_status").toString().toStdString();
        status_info.image_path_  = query.value("image").toString().toStdString();
        status_info.is_banned_   = query.value("is_banned").toBool();

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

    std::vector<Notification> retrieve_notifications(QSqlQuery& query, int count) {
        std::vector<Notification> notifications;
        Notification notification;

        do {
            notification.id_ = query.value("ID").toInt();
            notification.receiver_id_ = query.value("receiver_id").toInt();
            notification.text_ = query.value("text").toString().toStdString();

            notifications.push_back(notification);
            --count;

        } while (query.next() && count);

        return notifications;
    }

    std::vector<std::shared_ptr<IMessage>> retrieve_ind_messages(QSqlQuery &query, QSqlQuery &content_query) {
        if (query.size() < 0) {
            throw std::runtime_error("retrieve_ind_messages failed: query size is negative.\n");
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
                content_query.prepare("SELECT * FROM IND_AUDIO_MESSAGE_CONTENTS "
                                      "WHERE msg_id=:msg_id; ");
                content_query.bindValue(":msg_id", message_ptr->get_id());

                if (!content_query.exec() || !content_query.next()) {
                    std::cerr << content_query.lastError().text().toStdString() << '\n';
                    continue;
                }

                std::shared_ptr<AudioMessageContent> text_msg_content_ptr = std::make_shared<AudioMessageContent>();
                text_msg_content_ptr->set_audio_path(content_query.value("audio_path").toString().toStdString());
                message_ptr->set_msg_content(text_msg_content_ptr);
            } else if (msg_type == "image") {
                content_query.prepare("SELECT * FROM IND_IMAGE_MESSAGE_CONTENTS "
                                      "WHERE msg_id=:msg_id; ");
                content_query.bindValue(":msg_id", message_ptr->get_id());

                if (!content_query.exec() || !content_query.next()) {
                    std::cerr << content_query.lastError().text().toStdString() << '\n';
                    continue;
                }

                std::shared_ptr<ImageMessageContent> text_msg_content_ptr = std::make_shared<ImageMessageContent>();
                text_msg_content_ptr->set_image_path(content_query.value("image_path").toString().toStdString());
                message_ptr->set_msg_content(text_msg_content_ptr);
            }

            messages.push_back(message_ptr);
        } while (query.next());

        return messages;
    }

    std::vector<std::shared_ptr<IMessage>> retrieve_channel_messages(QSqlQuery &query, QSqlQuery &content_query) {
        if (query.size() < 0) {
            throw std::runtime_error("retrieve_channel_messages failed: query size is negative.\n");
        }

        std::vector<std::shared_ptr<IMessage>> messages;
        std::string msg_type;
        unsigned long long channel_id = query.value("channel_id").toULongLong();


        do {
            std::shared_ptr<ChannelMessage> message_ptr = std::make_shared<ChannelMessage>();
            message_ptr->set_id(query.value("ID").toULongLong());
            message_ptr->set_send_datetime(query.value("send_datetime").toDateTime());
            message_ptr->set_channel_id(channel_id);

            msg_type = query.value("content_type").toString().toStdString();
            if (msg_type == "text") {
                content_query.prepare("SELECT * FROM CHANNEL_TEXT_MESSAGE_CONTENTS "
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
                content_query.prepare("SELECT * FROM CHANNEL_AUDIO_MESSAGE_CONTENTS "
                                      "WHERE msg_id=:msg_id; ");
                content_query.bindValue(":msg_id", message_ptr->get_id());

                if (!content_query.exec() || !content_query.next()) {
                    std::cerr << content_query.lastError().text().toStdString() << '\n';
                    continue;
                }

                std::shared_ptr<AudioMessageContent> text_msg_content_ptr = std::make_shared<AudioMessageContent>();
                text_msg_content_ptr->set_audio_path(content_query.value("audio_path").toString().toStdString());
                message_ptr->set_msg_content(text_msg_content_ptr);
            } else if (msg_type == "image") {
                content_query.prepare("SELECT * FROM CHANNEL_IMAGE_MESSAGE_CONTENTS "
                                      "WHERE msg_id=:msg_id; ");
                content_query.bindValue(":msg_id", message_ptr->get_id());

                if (!content_query.exec() || !content_query.next()) {
                    std::cerr << content_query.lastError().text().toStdString() << '\n';
                    continue;
                }

                std::shared_ptr<ImageMessageContent> text_msg_content_ptr = std::make_shared<ImageMessageContent>();
                text_msg_content_ptr->set_image_path(content_query.value("image_path").toString().toStdString());
                message_ptr->set_msg_content(text_msg_content_ptr);
            }

            messages.push_back(message_ptr);
        } while (query.next());

        return messages;
    }
}
