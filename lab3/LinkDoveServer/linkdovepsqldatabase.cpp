#include "linkdovepsqldatabase.h"

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

LinkDovePSQLDataBase::LinkDovePSQLDataBase(const std::string &connection_name)
    : connection_name_(connection_name)
{
    setup();
}

LinkDovePSQLDataBase::~LinkDovePSQLDataBase() {
    data_base_.close();
    QSqlDatabase::removeDatabase(data_base_.databaseName());
}

void LinkDovePSQLDataBase::setup() {
    data_base_ = QSqlDatabase::addDatabase("QPSQL", connection_name_.c_str());
    data_base_.setHostName("localhost");
    data_base_.setUserName("frog");
    data_base_.setPassword("12345");
    data_base_.setDatabaseName("linkdovedatabase");

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

bool LinkDovePSQLDataBase::setup_tables() {
    QSqlQuery query(data_base_);



    query.exec("DROP PROCEDURE add_user;");
    bool is_ok = query.exec("DO $$ BEGIN "
                            "CREATE TYPE action_type AS ENUM('register', 'login', 'logout', 'edit profile', 'banned', 'unbanned');"
                            "EXCEPTION "
                            "WHEN duplicate_object THEN null;"
                            "END $$;");

    if (!is_ok) {
        std::cerr << "Failed to setup action_type type: " << query.lastError().text().toStdString() << '\n';
        return false;
    }

    is_ok = query.exec(" CREATE TABLE IF NOT EXISTS Consumer ("
                       "id serial PRIMARY KEY,"
                       "username varchar(40) NOT NULL UNIQUE,"
                       "email varchar(40) NOT NULL UNIQUE,"
                       "password varchar(40) NOT NULL CHECK(LENGTH(password) > 8 AND password ~* '[a-zA-Z]' AND password ~* '[0-9]'),"
                       "is_banned bool DEFAULT false);  ");

    if (!is_ok) {
        std::cerr << "Failed to setup Consumer table: " << query.lastError().text().toStdString() << '\n';
        return false;
    }

    is_ok = query.exec("CREATE TABLE IF NOT EXISTS Action ("
                       "id serial,"
                       "consumer integer NOT NULL,"
                       "type action_type,"
                       "time timestamp DEFAULT(now()),"
                       "PRIMARY KEY (id, time),"
                       "FOREIGN KEY (consumer) REFERENCES Consumer(id)"
                       "); ");
    if (!is_ok) {
        std::cerr << "Failed to setup Action table: " << query.lastError().text().toStdString() << '\n';
        return false;
    }

    is_ok = query.exec("CREATE TABLE IF NOT EXISTS Answer ("
                       "id serial PRIMARY KEY,"
                       "receiver integer NOT NULL,"
                       "description text NOT NULL,"
                       "FOREIGN KEY (receiver) REFERENCES Consumer(id)"
                       "); ");

    if (!is_ok) {
        std::cerr << "Failed to setup Answer table: " << query.lastError().text().toStdString() << '\n';
        return false;
    }

    is_ok = query.exec(" DO $$ BEGIN "
                       "CREATE TYPE broadchat_type AS ENUM('chat', 'channel');"
                       "EXCEPTION "
                       "WHEN duplicate_object THEN null;"
                       "END $$;");

    if (!is_ok) {
        std::cerr << "Failed to setup broadchat_type type:: " << query.lastError().text().toStdString() << '\n';
        return false;
    }

    is_ok = query.exec(" CREATE TABLE IF NOT EXISTS Broadchat ("
                       "id serial PRIMARY KEY,"
                       "owner integer NOT NULL,"
                       "name varchar(64) NOT NULL UNIQUE,"
                       "type broadchat_type,"
                       "is_private bool DEFAULT(false),"
                       "FOREIGN KEY (owner) REFERENCES Consumer(id)); ");

    if (!is_ok) {
        std::cerr << "Failed to setup Broadchat table: " << query.lastError().text().toStdString() << '\n';
        return false;
    }

    is_ok = query.exec("CREATE TABLE IF NOT EXISTS BannedInterlocutor ("
                       "from_id integer NOT NULL,"
                       "to_id integer NOT NULL CHECK(from_id != to_id),"
                       "FOREIGN KEY (from_id) REFERENCES Consumer(id) ON DELETE CASCADE,"
                       "FOREIGN KEY (to_id) REFERENCES Consumer(id) ON DELETE CASCADE"
                       ");");

    if (!is_ok) {
        std::cerr << "Failed to setup BannedInterlocutor table: " << query.lastError().text().toStdString() << '\n';
        return false;
    }

    is_ok = query.exec("CREATE TABLE IF NOT EXISTS BroadcastNotification ("
                       "id serial PRIMARY KEY,"
                       "description text NOT NULL,"
                       "time timestamp NOT NULL DEFAULT(NOW()));");

    if (!is_ok) {
        std::cerr << "Failed to setup BroadcastNotification table: " << query.lastError().text().toStdString() << '\n';
        return false;
    }

    is_ok = query.exec("CREATE TABLE IF NOT EXISTS BroadchatBanned ("
                       "id serial PRIMARY KEY,"
                       "broad_chat integer NOT NULL,"
                       "banned_consumer integer NOT NULL,"
                       "FOREIGN KEY (broad_chat) REFERENCES Broadchat(id) ON DELETE CASCADE,"
                       "FOREIGN KEY (banned_consumer) REFERENCES Consumer(id) ON DELETE CASCADE); ");

    if (!is_ok) {
        std::cerr << "Failed to setup BroadchatBanned table: " << query.lastError().text().toStdString() << '\n';
        return false;
    }

    is_ok = query.exec("DO $$ BEGIN "
                       "CREATE TYPE msg_type as ENUM('text', 'image', 'audio');"
                       "EXCEPTION "
                       "WHEN duplicate_object THEN null;"
                       "END $$;");

    if (!is_ok) {
        std::cerr << "Failed to setup msg_type type: " << query.lastError().text().toStdString() << '\n';
    }

    is_ok = query.exec("CREATE TABLE IF NOT EXISTS BroadchatParticipant ("
                       "id serial PRIMARY KEY,"
                       "broad_chat integer NOT NULL,"
                       "participant integer NOT NULL,"
                       "FOREIGN KEY (broad_chat) REFERENCES Broadchat(id) ON DELETE CASCADE,"
                       "FOREIGN KEY (participant) REFERENCES Consumer(id) ON DELETE CASCADE,"
                       "CONSTRAINT unique_part_in_chat UNIQUE(broad_chat, participant) ); ");

    if (!is_ok) {
        std::cerr << "Failed to setup BroadchatParticipant table: " << query.lastError().text().toStdString() << '\n';
        return false;
    }

    is_ok = query.exec(" CREATE TABLE IF NOT EXISTS BroadchatMessage ("
                       "id serial, "
                       "broad_chat integer NOT NULL,"
                       "sender integer NOT NULL,"
                       "time timestamp DEFAULT(now()),"
                       "content_type msg_type,"
                       "info text NOT NULL,"
                       "PRIMARY KEY (id),"
                       "FOREIGN KEY (broad_chat, sender) REFERENCES BroadchatParticipant(broad_chat, participant) ON DELETE CASCADE); ");

    if (!is_ok) {
        std::cerr << "Failed to setup BroadchatMessage table: " << query.lastError().text().toStdString() << '\n';
        return false;
    }

    is_ok = query.exec(" CREATE TABLE IF NOT EXISTS BroadchatRequest ("
                       "id serial PRIMARY KEY,"
                       "broad_chat integer NOT NULL,"
                       "consumer integer NOT NULL,"
                       "FOREIGN KEY (broad_chat) REFERENCES Broadchat(id) ON DELETE CASCADE,"
                       "FOREIGN KEY (consumer) REFERENCES Consumer(id) ON DELETE CASCADE);  ");

    if (!is_ok) {
        std::cerr << "Failed to setup BroadchatRequest table: " << query.lastError().text().toStdString() << '\n';
        return false;
    }

    is_ok = query.exec(" CREATE TABLE IF NOT EXISTS Complaint ("
                       "id serial PRIMARY KEY,"
                       "sender integer NOT NULL,"
                       "description text NOT NULL,"
                       "FOREIGN KEY (sender) REFERENCES Consumer(id)); ");

    if (!is_ok) {
        std::cerr << "Failed to setup Complaint table: " << query.lastError().text().toStdString() << '\n';
        return false;
    }

    is_ok = query.exec(" CREATE OR REPLACE VIEW BannedConsumer AS "
                       "SELECT * "
                       "FROM Consumer "
                       "WHERE is_banned=true; ");

    if (!is_ok) {
        std::cerr << "Failed to setup BannedConsumer view: " << query.lastError().text().toStdString() << '\n';
        return false;
    }

    is_ok = query.exec(" CREATE TABLE IF NOT EXISTS Country ("
                       "id serial PRIMARY KEY,"
                       "name varchar(128) NOT NULL UNIQUE);  ");

    if (!is_ok) {
        std::cerr << "Failed to setup Country table: " << query.lastError().text().toStdString() << '\n';
        return false;
    }

    is_ok = query.exec(" CREATE TABLE IF NOT EXISTS IndividualMessage ("
                       "id serial, "
                       "sender integer NOT NULL,"
                       "receiver integer NOT NULL CHECK(sender != receiver),"
                       "time timestamp DEFAULT(now()),"
                       "content_type msg_type,"
                       "info text NOT NULL,"
                       "PRIMARY KEY (id),"
                       "FOREIGN KEY (sender) REFERENCES Consumer(id) ON DELETE CASCADE, "
                       "FOREIGN KEY (receiver) REFERENCES Consumer(id) ON DELETE CASCADE); ");

    if (!is_ok) {
        std::cerr << "Failed to setup IndividualMessage table: " << query.lastError().text().toStdString() << '\n';
        return false;
    }

    is_ok = query.exec(" CREATE TABLE IF NOT EXISTS Profile ("
                       "id serial PRIMARY KEY,"
                       "status varchar(256),"
                       "avatar varchar(400) NOT NULL DEFAULT('media/avatars/default_avatar.png'),"
                       "birthday date NOT NULL CHECK(birthday <= (NOW() - '14 years'::interval)),"
                       "country integer NOT NULL,"
                       "consumer integer NOT NULL UNIQUE,"
                       "FOREIGN KEY (country) REFERENCES Country(id) ON DELETE RESTRICT,"
                       "FOREIGN KEY (consumer) REFERENCES Consumer(id) ON DELETE CASCADE);  ");

    if (!is_ok) {
        std::cerr << "Failed to setup Profile table: " << query.lastError().text().toStdString() << '\n';
        return false;
    }

    is_ok = query.exec(" CREATE OR REPLACE FUNCTION on_message_insert() RETURNS trigger AS $$"
                       "DECLARE"
                       "    banned_words TEXT ARRAY DEFAULT ARRAY ['%наркотик%', '%оружие%', '%дура%'];"
                       "BEGIN"
                       "    IF NEW.content_type = 'text' AND NEW.info ILIKE ANY (banned_words) THEN "
                       "        RAISE EXCEPTION 'Сообщение содержит запрещенное слово';"
                       "    END IF;"
                       ""
                       "    RETURN NEW;"
                       "END;"
                       "$$ LANGUAGE plpgsql;  ");

    if (!is_ok) {
        std::cerr << "Failed to setup on_message_insert function: " << query.lastError().text().toStdString() << '\n';
        return false;
    }

    is_ok = query.exec(" CREATE OR REPLACE  TRIGGER BroadchatMessage_INSERT "
                       "BEFORE INSERT ON BroadchatMessage "
                       "FOR EACH ROW "
                       "EXECUTE FUNCTION on_message_insert(); ");

    if (!is_ok) {
        std::cerr << "Failed to setup BroadchatMessage_INSERT trigger: " << query.lastError().text().toStdString() << '\n';
        return false;
    }

    is_ok = query.exec(" CREATE OR REPLACE TRIGGER IndividualMessage_INSERT "
                       "BEFORE INSERT ON BroadchatMessage "
                       "FOR EACH ROW "
                       "EXECUTE FUNCTION on_message_insert(); ");

    if (!is_ok) {
        std::cerr << "Failed to setup IndividualMessage_INSERT trigger: " << query.lastError().text().toStdString() << '\n';
        return false;
    }

    is_ok = query.exec(" CREATE OR REPLACE  FUNCTION on_profile_update() RETURNS trigger AS $$"
                       "DECLARE"
                       "    banned_words TEXT ARRAY DEFAULT ARRAY ['%наркотик%', '%оружие%', '%дура%'];"
                       "BEGIN"
                       "    IF NEW.status ILIKE ANY (banned_words) THEN "
                       "        RAISE EXCEPTION 'Статус содержит запрещённое слово';"
                       "    END IF;"
                       "    RETURN NEW;"
                       "END;"
                       "$$ LANGUAGE plpgsql;");

    if (!is_ok) {
        std::cerr << "Failed to setup on_profile_update function: " << query.lastError().text().toStdString() << '\n';
        return false;
    }

    is_ok = query.exec("CREATE OR REPLACE  TRIGGER Profile_UPDATE "
                       "BEFORE UPDATE ON Profile "
                       "FOR EACH ROW "
                       "EXECUTE FUNCTION on_profile_update();");

    if (!is_ok) {
        std::cerr << "Failed to setup Profile_UPDATE trigger: " << query.lastError().text().toStdString() << '\n';
        return false;
    }

    is_ok = query.exec("CREATE OR REPLACE PROCEDURE add_action(id integer, user_action action_type) "
                       "LANGUAGE SQL "
                       "AS $$ "
                       "    INSERT INTO Action(consumer, type) "
                       "    VALUES (id, user_action); "
                       "$$;");

    if (!is_ok) {
        std::cerr << "Failed to setup add_action procedure: " << query.lastError().text().toStdString() << '\n';
        return false;
    }

    is_ok = query.exec("CREATE OR REPLACE TRIGGER Consumer_UPDATE "
                       "AFTER INSERT ON Consumer "
                       "FOR EACH ROW "
                       "EXECUTE FUNCTION on_user_registration();");

    if (!is_ok) {
        std::cerr << "Failed to setup Consumer_UPDATE trigger: " << query.lastError().text().toStdString() << '\n';
        return false;
    }

    is_ok = query.exec("CREATE OR REPLACE FUNCTION on_user_registration() RETURNS trigger AS $$ "
                       "BEGIN "
                       "    call add_action(NEW.id, 'register'); "
                       "    call add_action(NEW.id, 'login'); "
                       "    RETURN NEW; "
                       "END; "
                       "$$ LANGUAGE plpgsql;");

    if (!is_ok) {
        std::cerr << "Failed to setup on_user_registration function: " << query.lastError().text().toStdString() << '\n';
        return false;
    }

    is_ok = query.exec("CREATE OR REPLACE PROCEDURE add_user(username varchar(40), email varchar(40), password varchar(40), birthday date, country_id integer) "
                       "AS $$ "
                       "DECLARE "
                       "   v_id integer; "
                       "BEGIN "
                       "    INSERT INTO Consumer(username, email, password) "
                       "    VALUES (username, email, password) "
                       "    RETURNING id INTO v_id; "
                       "    INSERT INTO Profile (birthday, country, consumer) "
                       "    VALUES "
                       "        (birthday, country_id, v_id); "
                       "END; "
                       "$$ LANGUAGE plpgsql;");

    if (!is_ok) {
        std::cerr << "Failed to setup add_user procedure: " << query.lastError().text().toStdString() << '\n';
        return false;
    }

    is_ok = query.exec("CREATE OR REPLACE FUNCTION update_userinfo(id integer, username varchar(40), email varchar(40), birthday date, text_status varchar(256), avatar varchar(400)) "
                       "RETURNS integer AS $$ "
                       "BEGIN "
                       "    UPDATE Consumer SET (username, email) = (username, email) "
                       "    WHERE id=id; "
                       "    UPDATE Profile SET (status, avatar) = (text_status, avatar) "
                       "    WHERE consumer=id; "
                       "    return id; "
                       "END; "
                       "$$ LANGUAGE plpgsql;  ");

    if (!is_ok) {
        std::cerr << "Failed to setup update_userinfo function: " << query.lastError().text().toStdString() << '\n';
        return false;
    }

    is_ok = query.exec("CREATE OR REPLACE PROCEDURE ban_user(username varchar(40), is_banned boolean) "
                       "LANGUAGE SQL "
                       "AS $$ "
                       "    UPDATE Consumer SET is_banned = is_banned "
                       "    WHERE username=username; "
                       "$$; ");

    if (!is_ok) {
        std::cerr << "Failed to setup ban_user procedure: " << query.lastError().text().toStdString() << '\n';
        return false;
    }

    is_ok = query.exec("CREATE OR REPLACE PROCEDURE update_banned_interlocutor_status(from_id integer, to_id integer, is_banned boolean) "
                       "AS $$ "
                       "BEGIN "
                       "    IF is_banned=true THEN "
                       "        INSERT INTO BannedInterlocutor(from_id, to_id) "
                       "        VALUES "
                       "            (from_id, to_id); "
                       "    ELSE  "
                       "        DELETE FROM BannedInterlocutor "
                       "        WHERE to_id=to_id AND from_id=from_id; "
                       "    END IF; "
                       "END; "
                       "$$ LANGUAGE PLPGSQL;");

    if (!is_ok) {
        std::cerr << "Failed to setup update_banned_interlocutor_status procedure: " << query.lastError().text().toStdString() << '\n';
        return false;
    }

    is_ok = query.exec("CREATE OR REPLACE PROCEDURE update_banned_broadchat_status(broad_chat integer, banned_consumer integer, is_banned boolean) "
                       "AS $$ "
                       "BEGIN "
                       "    IF is_banned THEN "
                       "        INSERT INTO BroadchatBanned (broad_chat, banned_consumer) "
                       "        VALUES  "
                       "            (broad_chat, banned_consumer); "
                       "    ELSE  "
                       "        DELETE FROM BroadchatBanned "
                       "        WHERE broad_chat=broad_chat AND banned_consumer=banned_consumer; "
                       "    END IF; "
                       "END; "
                       "$$ LANGUAGE PLPGSQL;");

    if (!is_ok) {
        std::cerr << "Failed to setup update_banned_broadchat_status procedure: " << query.lastError().text().toStdString() << '\n';
        return false;
    }

    is_ok = query.exec("CREATE OR REPLACE PROCEDURE add_channel_message(broad_chat integer, content_type msg_type, info text) "
                       "AS $$ "
                       "DECLARE "
                       "    sender integer; "
                       "BEGIN "
                       "    SELECT owner INTO sender FROM Broadchat "
                       "    WHERE id=broad_chat; "
                       "    INSERT INTO BroadchatMessage (broad_chat, sender, content_type, info) "
                       "    VALUES  "
                       "        (broad_chat, sender, content_type, info); "
                       "END; "
                       "$$ LANGUAGE PLPGSQL;");

    if (!is_ok) {
        std::cerr << "Failed to setup add_channel_message procedure: " << query.lastError().text().toStdString() << '\n';
        return false;
    }

    is_ok = query.exec("CREATE OR REPLACE PROCEDURE add_broadchat(owner integer, chat_name varchar(64), chat_type broadchat_type, is_private boolean) "
                       "AS $$ "
                       "BEGIN"
                       "   INSERT INTO Broadchat (owner, name, type, is_private) "
                       "   VALUES "
                       "        (owner, chat_name, chat_type, is_private); "
                       "    INSERT INTO BroadchatParticipant (broad_chat, participant) "
                       "    VALUES "
                       "        (broad_chat, participant); "
                       "END; "
                       "$$ LANGUAGE PLPGSQL;");

    if (!is_ok) {
        std::cerr << "Failed to setup add_broadchat procedure: " << query.lastError().text().toStdString() << '\n';
        return false;
    }

    is_ok = query.exec("CREATE OR REPLACE PROCEDURE add_broadchat_participant(broad_chat integer, participant integer) "
                       "AS $$  "
                       "BEGIN "
                       "    INSERT INTO BroadchatParticipant (broad_chat, participant) "
                       "    VALUES  "
                       "        (broad_chat, participant); "
                       "    DELETE FROM BroadchatRequest "
                       "    WHERE id=broad_chat AND consumer=participant AND is_private=true; "
                       "END; "
                       "$$ LANGUAGE PLPGSQL;");

    if (!is_ok) {
        std::cerr << "Failed to setup add_broadchat_participant procedure: " << query.lastError().text().toStdString() << '\n';
        return false;
    }

    is_ok = query.exec("CREATE OR REPLACE PROCEDURE add_chat_message(broad_chat integer, sender integer, content_type msg_type, info text) "
                       "AS $$ "
                       "BEGIN "
                       "    INSERT INTO BroadchatMessage (broad_chat, sender, content_type, info) "
                       "    VALUES  "
                       "        (broad_chat, sender, content_type, info); "
                       "END; "
                       "$$ LANGUAGE PLPGSQL;");

    if (!is_ok) {
        std::cerr << "Failed to setup add_chat_message procedure: " << query.lastError().text().toStdString() << '\n';
        return false;
    }

    is_ok = query.exec("INSERT INTO Country (name) "
                       "VALUES "
                       "    ('Беларусь'), "
                       "    ('Россия'), "
                       "    ('Польша'), "
                       "    ('Украина') "
                       "ON CONFLICT DO NOTHING;");

    if (!is_ok) {
        std::cerr << "Failed to setup Country table content: " << query.lastError().text().toStdString() << '\n';
        return false;
    }


    return true;
}

bool LinkDovePSQLDataBase::register_user(const UserInfo& info) {
    QSqlQuery query(data_base_);
    QString str_query = QString("CALL add_user(\'%1\', \'%2\', \'%3\', \'%4\', %5)")
                        .arg(info.status_info_.username_.c_str())
                        .arg(info.status_info_.email_.c_str())
                        .arg(info.password_.c_str())
                        .arg(info.status_info_.birthday_.toString("yyyy-MM-dd"))
                        .arg(1);

    if (!query.exec(str_query)) {
        std::cerr << "Failed to setup Country table content: " << query.lastError().text().toStdString() << '\n';
        return false;
    } else {
        // если вставка была успешна, то row affected > 0, иначе row affected == 0 (false).
        return true;
    }
}

bool LinkDovePSQLDataBase::login_user(const LoginInfo& info) {
    QSqlQuery query(data_base_);
    query.prepare("SELECT "
                  "EXISTS (SELECT 1 FROM Consumer "
                  "        WHERE username=:username AND email=:email AND password=:password) AS PassedAuthentification;");

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

bool LinkDovePSQLDataBase::update_user(const StatusInfo& status_info) {
    StatusInfo inner_info;
    bool is_username_changed = false,
         is_email_changed    = false;
    try { // Такого пользователя нет
        inner_info = get_status_info(status_info.id_);
    } catch(std::runtime_error& ex) {
        return false;
    }

    QSqlQuery query(data_base_);

    query.prepare("DO $$ BEGIN "
                  "PERFORM \"update_userinfo\"(:id, :username, :email, :birthday, :text_status, :avatar); "
                  "END $$;");
    query.bindValue(":id",          status_info.id_);
    query.bindValue(":username",    status_info.username_.c_str());
    query.bindValue(":email",       status_info.email_.c_str());
    query.bindValue(":birthday",    status_info.birthday_.toString("yyyy-MM-dd"));
    query.bindValue(":text_status", status_info.text_status_.c_str());
    query.bindValue(":avatar",      status_info.image_path_.c_str());

    if (!query.exec()) {
        std::cerr  << query.lastError().text().toStdString() << '\n';
        return false;
    } else {
        // если изменение было успешно, то row affected > 0, иначе row affected == 0 (false).
        int num = query.numRowsAffected();
        // Если num == 0, а поля никнейма или почты изменились, то на вход пришел дубликат.
        if (num == 0) {
            return false;
        } else {
            return true;
        }
    }
}

bool LinkDovePSQLDataBase::ban_user(const std::string &username, bool is_ban) {
    QSqlQuery query(data_base_);
    query.prepare("CALL ban_user(:username, :is_banned);");

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

bool LinkDovePSQLDataBase::ban_ind_user(unsigned long long from_id, unsigned long long to_id, bool is_ban) {
    QSqlQuery query(data_base_);

    query.prepare("CALL update_banned_interlocutor_status(:from_id, :to_id, :is:banned);");

    query.bindValue(":from_id", from_id);
    query.bindValue(":to_id", to_id);
    query.bindValue(":is_banned", is_ban);

    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString() << '\n';
        return false;
    } else {
        // если блокировка была успешна, то row affected > 0, иначе row affected == 0 (false).
        return query.numRowsAffected();
    }
}

bool LinkDovePSQLDataBase::ban_chat_user(unsigned long long chat_id, unsigned long long user_id, bool is_ban) {
    QSqlQuery query(data_base_);

    query.prepare("CALL update_banned_interlocutor_status(:chat_id, :user_id, :is:banned);");

    query.bindValue(":chat_id", chat_id);
    query.bindValue(":user_id", user_id);
    query.bindValue(":is_banned", is_ban);

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

bool LinkDovePSQLDataBase::is_banned_ind_user(unsigned long long from_id, unsigned long long to_id) {
    QSqlQuery query(data_base_);

    query.prepare(" SELECT "
                  " EXISTS (SELECT * FROM BannedInterlocutor WHERE to_id=:to_id AND from_id=:from_id) As IsBanned; ");

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

bool LinkDovePSQLDataBase::is_banned_chat_user(unsigned long long chat_id, unsigned long long user_id) {
    QSqlQuery query(data_base_);

    query.prepare(" SELECT "
                  " EXISTS (SELECT * FROM BroadchatBanned WHERE banned_consumer=:user_id AND broad_chat=:chat_id) As IsBanned; ");

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

bool LinkDovePSQLDataBase::get_user_banned_status(const std::string &username) {
    QSqlQuery query(data_base_);
    query.prepare(" SELECT is_banned FROM Consumer "
                  " WHERE username=:username; ");

    query.bindValue(":username", username.c_str());

    if (!query.exec() || !query.next()) {
        std::cerr << query.lastError().text().toStdString() << '\n';
        throw std::runtime_error("Failed to check user banned status");
    } else {
        return query.value("is_banned").toBool();
    }
}

std::vector<std::string> LinkDovePSQLDataBase::get_banned_interlocutors(unsigned long long id) {
    QSqlQuery query(data_base_);
    query.prepare(" SELECT * FROM BannedInterlocutor "
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

bool LinkDovePSQLDataBase::add_complaint(const Complaint& complaint) {
    QSqlQuery query(data_base_);
    query.prepare("INSERT INTO Complaint (sender, description) "
                  "VALUES "
                  "     (:sender, :description);");

    query.bindValue(":sender", complaint.sender_id_);
    query.bindValue(":description", complaint.text_.c_str());

    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString() << '\n';
        return false;
    } else {
        // если вставка была успешна, то row affected > 0, иначе row affected == 0 (false).
        return query.numRowsAffected();
    }
}

bool LinkDovePSQLDataBase::del_complaint(unsigned long long complaint_id) {
    QSqlQuery query(data_base_);
    query.prepare("DELETE FROM Complaint "
                  "WHERE id=:id;");

    query.bindValue(":id", complaint_id);
    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString() << '\n';
        return false;
    } else {
        // если удаление было успешным, то row affected > 0, иначе row affected == 0 (false).
        return query.numRowsAffected();
    }
}

int LinkDovePSQLDataBase::get_complaints_count() {
    QSqlQuery query(data_base_);
    query.prepare("SELECT COUNT(*) FROM Complaint; ");

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

std::vector<Complaint> LinkDovePSQLDataBase::get_complaints(int count) {
    QSqlQuery query(data_base_);
    query.prepare("SELECT * FROM Complaint");

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

bool LinkDovePSQLDataBase::add_answer(unsigned long long user_id, const std::string &text) {
    QSqlQuery query(data_base_);

    query.prepare("INSERT INTO Answer(receiver, description) "
                  "VALUES "
                  "    (:receiver, :description); ");

    query.bindValue(":receiver", user_id);
    query.bindValue(":description", text.c_str());

    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString() << '\n';
        return false;
    } else {
        // если вставка была успешна, то row affected > 0, иначе row affected == 0 (false).
        return query.numRowsAffected();
    }
}

std::vector<Notification> LinkDovePSQLDataBase::get_notifications(unsigned long long user_id, int count) {
    QSqlQuery query(data_base_);
    query.prepare(" SELECT * FROM Answer "
                  " WHERE receiver=:user_id; ");

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

bool LinkDovePSQLDataBase::delete_notification(unsigned long long id) {
    QSqlQuery query(data_base_);

    query.prepare(" DELETE FROM Answer "
                  " WHERE id=:id; ");

    query.bindValue(":id", id);

    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString() << '\n';
        return false;
    } else {
        // если удаление было успешно, то row affected > 0, иначе row affected == 0 (false).
        return query.numRowsAffected();
    }
}

bool LinkDovePSQLDataBase::add_ind_message(const IMessage& msg) {
    QSqlQuery query(data_base_);
    unsigned long long msg_id = 0;

    // используем мьютекс для того, чтобы во время транзакции не было модификации данных
    std::unique_lock<std::mutex> unique_mtx(modify_mutex);

    // используем транзакцию, чтобы в случае ошибки добавления сообщения данные в БД были
    // в согласованном состоянии
    if (data_base_.transaction()) {
        query.prepare(" INSERT INTO IndividualMessage(sender, receiver, content_type, info) "
                      " VALUES "
                      "    (:sender, :receiver, :content_type, :info); ");

        query.bindValue(":sender", static_cast<const IndividualMessage&>(msg).get_msg_edges().first);
        query.bindValue(":receiver", static_cast<const IndividualMessage&>(msg).get_msg_edges().second);

        switch (msg.get_msg_content()->get_msg_content_type()) {
            case TEXT_MSG_TYPE: {
                query.bindValue(":content_type", "text");
                break;
            }
            case IMAGE_MSG_TYPE: {
                query.bindValue(":content_type", "image");
                break;
            }
            case AUDIO_MSG_TYPE: {
                query.bindValue(":content_type", "audio");
                break;
            }
        }

        query.bindValue(":info",  msg.get_msg_content()->get_raw_data());

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

bool LinkDovePSQLDataBase::delete_ind_message(const IMessage &msg) {
    QSqlQuery query(data_base_);

    query.prepare(" DELETE FROM IndividualMessage "
                  " WHERE id=:id; ");

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

bool LinkDovePSQLDataBase::add_chnnl_message(const IMessage& msg) {
    QSqlQuery query(data_base_);
    unsigned long long msg_id = 0;

    // используем мьютекс для того, чтобы во время транзакции не было модификации данных
    std::unique_lock<std::mutex> unique_mtx(modify_mutex);

    // используем транзакцию, чтобы в случае ошибки добавления сообщения данные в БД были
    // в согласованном состоянии
    if (data_base_.transaction()) {
        query.prepare("CALL add_channel_message(:broad_chat, :content_type, :info);");

        query.bindValue(":broad_chat", static_cast<const ChannelMessage&>(msg).get_channel_id());
        //query.bindValue(":sender", static_cast<const ChannelMessage&>(msg).get_channel_id()); TO DO FIX

        switch (msg.get_msg_content()->get_msg_content_type()) {
            case TEXT_MSG_TYPE: {
                query.bindValue(":content_type", "text");
                break;
            }
            case IMAGE_MSG_TYPE: {
                query.bindValue(":content_type", "image");
                break;
            }
            case AUDIO_MSG_TYPE: {
                query.bindValue(":content_type", "audio");
                break;
            }
        }

        query.bindValue(":info",  msg.get_msg_content()->get_raw_data());


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

bool LinkDovePSQLDataBase::delete_chnnl_message(const IMessage& msg) {
    QSqlQuery query(data_base_);

    query.prepare(" DELETE FROM BroadchatMessage "
                  " WHERE id=:id; ");

    query.bindValue(":id", msg.get_id());

    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString() << '\n';
        return false;
    } else {
        // если удаление было успешно, то row affected > 0, иначе row affected == 0 (false).
        return query.numRowsAffected();
    }
}

std::vector<std::shared_ptr<IMessage>> LinkDovePSQLDataBase::get_ind_messages(unsigned long long sender_id, unsigned long long receiver_id) {
    QSqlQuery query(data_base_);

    query.prepare("SELECT * FROM IndividualMessage "
                  " WHERE sender=:sender_id and receiver=:receiver_id; ");

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

std::vector<StatusInfo> LinkDovePSQLDataBase::get_interlocutors(unsigned long long id) {
    QSqlQuery query(data_base_);

    query.prepare(" SELECT DISTINCT sender FROM IndividualMessage "
                  " WHERE receiver=:id; ");

    query.bindValue(":id", id);

    std::vector<StatusInfo> interlocutors;
    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString() << '\n';
        throw std::runtime_error("get_interlocutors failed due to query.exec");
    } else {
        while (query.next()) {
            interlocutors.push_back(get_status_info(query.value("sender").toULongLong()));
        }
    }

    query.prepare(" SELECT DISTINCT receiver FROM IndividualMessage "
                  " WHERE sender=:id; ");

    query.bindValue(":id", id);

    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString() << '\n';
        throw std::runtime_error("get_interlocutors failed due to query.exec");
    } else {
        while (query.next()) {
            interlocutors.push_back(get_status_info(query.value("receiver").toULongLong()));
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

bool LinkDovePSQLDataBase::delete_ind_chat(unsigned long long first_id, unsigned long long second_id) {
    QSqlQuery query(data_base_);
    query.prepare(" DELETE FROM IndividualMessage "
                  " WHERE "
                  " sender=:first_id AND receiver=:second_id "
                  " OR "
                  " sender=:second_id AND receiver=:first_id; ");

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

bool LinkDovePSQLDataBase::add_channel(const ChannelInfo &channel_info) {
    QSqlQuery query(data_base_);
    query.prepare("CALL add_broadchat(:owner, :chat_name, 'channel', :is_private);");

    query.bindValue(":owner", channel_info.owner_id_);
    query.bindValue(":chat_name", channel_info.name_.c_str());
    query.bindValue(":is_private", channel_info.is_private_);

    if (!query.exec()) {
            std::cerr << query.lastError().text().toStdString();
            return false;
    } else {
        std::cerr << "Failed to start transaction during adding new channel\n";
        return false;
    }
}

std::vector<ChannelInfo> LinkDovePSQLDataBase::get_channels(unsigned long long id) {
    QSqlQuery query(data_base_);
    query.prepare(" SELECT * FROM Broadchat "
                  " WHERE participant_id=:id AND type='channel");

    query.bindValue(":id", id);

    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString() << '\n';
        throw std::runtime_error("get_channels failed due to query.exec");
    }

    std::vector<ChannelInfo> channels;
    while (query.next()) {
        channels.push_back(get_channel(query.value("id").toULongLong()));
    }

    return channels;
}

ChannelInfo LinkDovePSQLDataBase::get_channel(const std::string &channel_name) {
    QSqlQuery query(data_base_);
    query.prepare(" SELECT * FROM Broadchat "
                  " WHERE name = :name AND type='channel'; ");

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

ChannelInfo LinkDovePSQLDataBase::get_channel(unsigned long long channel_id) {
    QSqlQuery query(data_base_);
    query.prepare(" SELECT * FROM Broadchat "
                  " WHERE id=:id AND type='channel'; ");

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

bool LinkDovePSQLDataBase::add_participant_to_channel(unsigned long long participant_id, unsigned long long channel_id) {
    QSqlQuery query(data_base_);
    query.prepare(" CALL add_broadchat_participant(:broad_chat, participant); ");

    query.bindValue(":broad_chat", channel_id);
    query.bindValue(":participant", participant_id);

    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString();
        return false;
    } else {
        // если вставка была успешна, то row affected > 0, иначе row affected == 0 (false).
        return query.numRowsAffected();
    }
}

bool LinkDovePSQLDataBase::request_participant_to_channel(unsigned long long user_id, unsigned long long channel_id) {
    QSqlQuery query(data_base_);
    query.prepare(" INSERT INTO BroadchatRequest (broad_chat, consumer) "
                  " VALUES "
                  "    (:broad_chat, :consumer); ");

    query.bindValue(":broad_chat", channel_id);
    query.bindValue(":consumer", user_id);

    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString();
        return false;
    } else {
        // если вставка была успешна, то row affected > 0, иначе row affected == 0 (false).
        return query.numRowsAffected();
    }
}

bool LinkDovePSQLDataBase::remove_request_channel(unsigned long long user_id, unsigned long long channel_id) {
    QSqlQuery query(data_base_);

    query.prepare(" DELETE FROM BroadchatRequest "
                  " WHERE id=:broad_chat AND consumer=:participant AND is_private=true; ");

    query.bindValue(":broad_chat", channel_id);
    query.bindValue(":participant", user_id);

    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString();
        return false;
    } else {
        // если удаление было успешно, то row affected > 0, иначе row affected == 0 (false).
        return query.numRowsAffected();
    }
}

bool LinkDovePSQLDataBase::request_participant_to_chat(unsigned long long user_id, unsigned long long chat_id) {
    QSqlQuery query(data_base_);
    query.prepare(" CALL add_broadchat_participant(:broad_chat, participant); ");

    query.bindValue(":broad_chat", chat_id);
    query.bindValue(":participant", user_id);

    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString();
        return false;
    } else {
        // если вставка была успешна, то row affected > 0, иначе row affected == 0 (false).
        return query.numRowsAffected();
    }
}

bool LinkDovePSQLDataBase::remove_request_chat(unsigned long long user_id, unsigned long long chat_id) {
    return request_participant_to_channel(user_id, chat_id);
}

bool LinkDovePSQLDataBase::is_channel_participant(unsigned long long participant_id, unsigned long long channel_id) {
    QSqlQuery query(data_base_);
    query.prepare(" SELECT "
                  " EXISTS (SELECT 1 FROM BroadchatParticipant "
                  "        WHERE broad_chat=:broad_chat AND participant=:participant) As IsParticipant; ");

    query.bindValue(":broad_chat", channel_id);
    query.bindValue(":participant", participant_id);

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

std::vector<std::shared_ptr<IMessage>> LinkDovePSQLDataBase::get_channel_messages(unsigned long long channel_id) {
    QSqlQuery query(data_base_);

    query.prepare("SELECT * FROM BroadchatMessage "
                  "WHERE broad_chat=:broad_chat "
                  "ORDER BY time;  ");

    query.bindValue(":broad_chat", channel_id);

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

std::vector<std::string> LinkDovePSQLDataBase::get_channel_participants(unsigned long long channel_id) {
    QSqlQuery query(data_base_);

    query.prepare(" SELECT participant FROM BroadchatParticipant "
                  " WHERE broad_chat=:broad_chat; ");

    query.bindValue(":broad_chat", channel_id);

    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString() << '\n';
        throw std::runtime_error("get_channel_requests failed due to query.exec");
    }

    std::vector<std::string> participants;
    while (query.next()) {
        participants.push_back(get_status_info(query.value("participant").toULongLong()).username_);
    }

    return participants;
}

std::vector<std::string> LinkDovePSQLDataBase::get_channel_requests(unsigned long long channel_id) {
    QSqlQuery query(data_base_);

    query.prepare(" SELECT consumer FROM BroadchatRequest "
                  " WHERE broad_chat=:broad_chat; ");

    query.bindValue(":broad_chat", channel_id);

    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString() << '\n';
        throw std::runtime_error("get_channel_requests failed due to query.exec");
    }

    std::vector<std::string> participants;
    while (query.next()) {
        participants.push_back(get_status_info(query.value("consumer").toULongLong()).username_);
    }

    return participants;
}

std::vector<std::string> LinkDovePSQLDataBase::get_chat_requests(unsigned long long chat_id) {
    return get_channel_requests(chat_id);
}

bool LinkDovePSQLDataBase::delete_channel(unsigned long long channel_id) {
    QSqlQuery query(data_base_);

    // Удаляем все текстовые сообщения
    query.prepare(" DELETE FROM Broadchat "
                  " WHERE id=:id; ");

    query.bindValue(":id", channel_id);

    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString() << '\n';
        return false;
    } else {
        // если удаление было успешным, то row affected > 0, иначе row affected == 0 (false).
        return query.numRowsAffected();
    }
}

bool LinkDovePSQLDataBase::quit_channel(unsigned long long user_id, unsigned long long channel_id) {
    QSqlQuery query(data_base_);
    query.prepare(" DELETE FROM BroadchatParticipant "
                  " WHERE broad_chat=:broad_chat AND participant=:participant; " );

    query.bindValue(":participant", user_id);
    query.bindValue(":broad_chat", channel_id);

    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString() << '\n';
        return false;
    } else {
        // если удаление было успешным, то row affected > 0, иначе row affected == 0 (false).
        return query.numRowsAffected();
    }
}

bool LinkDovePSQLDataBase::add_chat(const ChatInfo &chat_info) {
    QSqlQuery query(data_base_);

    query.prepare("CALL add_broadchat(:owner, :chat_name, 'chat', :is_private);");

    query.bindValue(":owner", chat_info.owner_id_);
    query.bindValue(":chat_name", chat_info.name_.c_str());
    query.bindValue(":is_private", chat_info.is_private_);

    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString() << '\n';
        return false;
    } else {
        // если вставка была успешна, то row affected > 0, иначе row affected == 0 (false).
        if (query.numRowsAffected()) {
            return true;
        } else {
            return false;
        }
    }
}

ChatInfo LinkDovePSQLDataBase::get_chat(const std::string &chat_name) {
    QSqlQuery query(data_base_);
    query.prepare(" SELECT * FROM Broadchat "
                  " WHERE name=:name AND type='chat'; ");

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

ChatInfo LinkDovePSQLDataBase::get_chat(unsigned long long id) {
    QSqlQuery query(data_base_);
    query.prepare(" SELECT * FROM Broadchat "
                  " WHERE id=:id AND type='chat'; ");

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

bool LinkDovePSQLDataBase::add_participant_to_chat(unsigned long long participant_id, unsigned long long chat_id) {
    return add_participant_to_channel(participant_id, chat_id);
}

bool LinkDovePSQLDataBase::is_chat_participant(unsigned long long participant_id, unsigned long long chat_id) {
    return is_channel_participant(participant_id, chat_id);
}

std::vector<ChatInfo> LinkDovePSQLDataBase::get_chats(unsigned long long id) {
    QSqlQuery query(data_base_);
    query.prepare(" SELECT * FROM Broadchat "
                  " WHERE participant_id=:id AND type='chat");

    query.bindValue(":id", id);

    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString() << '\n';
        throw std::runtime_error("get_chats failed due to query.exec");
    }

    std::vector<ChatInfo> chats;
    while (query.next()) {
        chats.push_back(get_chat(query.value("id").toULongLong()));
    }

    return chats;
}

bool LinkDovePSQLDataBase::delete_chat(unsigned long long chat_id) {
    QSqlQuery query(data_base_);

    query.prepare(" DELETE FROM Broadchat "
                  " WHERE id=:id; ");

    query.bindValue(":id", chat_id);
    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString() << '\n';
        return false;
    } else {
        // если удаление было успешным, то row affected > 0, иначе row affected == 0 (false).
        return query.numRowsAffected();
    }
}

bool LinkDovePSQLDataBase::quit_chat(unsigned long long user_id, unsigned long long chat_id) {
    return quit_channel(user_id, chat_id);
}

/*
    if (data_base_.transaction()) {
        query.prepare("CALL add_channel_message(:broad_chat, :content_type, :info);");

        query.bindValue(":broad_chat", static_cast<const ChannelMessage&>(msg).get_channel_id());
        //query.bindValue(":sender", static_cast<const ChannelMessage&>(msg).get_channel_id()); TO DO FIX

        switch (msg.get_msg_content()->get_msg_content_type()) {
            case TEXT_MSG_TYPE: {
                query.bindValue(":content_type", "text");
                break;
            }
            case IMAGE_MSG_TYPE: {
                query.bindValue(":content_type", "image");
                break;
            }
            case AUDIO_MSG_TYPE: {
                query.bindValue(":content_type", "audio");
                break;
            }
        }

        query.bindValue(":info",  msg.get_msg_content()->get_raw_data());


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
*/

bool LinkDovePSQLDataBase::add_chat_message(const IMessage& msg) {
    QSqlQuery query(data_base_);
    unsigned long long msg_id = 0;

    // используем мьютекс для того, чтобы во время транзакции не было модификации данных
    std::unique_lock<std::mutex> unique_mtx(modify_mutex);

    query.prepare("CALL add_chat_message(:broad_chat, :sender, :content_type, :info);");

    query.bindValue(":broad_chat", static_cast<const GroupMessage&>(msg).get_group_id());
    query.bindValue(":sender", static_cast<const GroupMessage&>(msg).get_owner_id());

    switch (msg.get_msg_content()->get_msg_content_type()) {
        case TEXT_MSG_TYPE: {
            query.bindValue(":content_type", "text");
            break;
        }
        case IMAGE_MSG_TYPE: {
            query.bindValue(":content_type", "image");
            break;
        }
        case AUDIO_MSG_TYPE: {
            query.bindValue(":content_type", "audio");
            break;
        }
    }

    query.bindValue(":info",  msg.get_msg_content()->get_raw_data());

    if (!query.exec()) {
        std::cerr << query.lastError().text().toStdString() << '\n';
        return false;
    }

    return true;
}

bool LinkDovePSQLDataBase::delete_chat_message(const IMessage& msg) {
    return delete_chnnl_message(msg);
}

// TODO FIX
std::vector<std::shared_ptr<IMessage>> LinkDovePSQLDataBase::get_chat_messages(unsigned long long chat_id) {
    QSqlQuery query(data_base_);

    query.prepare("SELECT * FROM BroadchatMessage "
                  "WHERE broad_chat=:broad_chat "
                  "ORDER BY time;  ");

    query.bindValue(":broad_chat", chat_id);

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

        do {
            user_info = get_status_info(query.value("sender").toULongLong());

            std::shared_ptr<GroupMessage> message_ptr = std::make_shared<GroupMessage>();
            message_ptr->set_id(query.value("id").toULongLong());
            message_ptr->set_send_datetime(query.value("time").toDateTime());
            message_ptr->set_group_id(chat_id);
            message_ptr->set_owner_id(query.value("sender").toULongLong());
            message_ptr->set_owner_name(user_info.username_);

            msg_type = query.value("content_type").toString().toStdString();
            if (msg_type == "text") {
                std::shared_ptr<TextMessageContent> text_msg_content_ptr = std::make_shared<TextMessageContent>();
                text_msg_content_ptr->set_text(content_query.value("info").toString().toStdString());
                message_ptr->set_msg_content(text_msg_content_ptr);
            } else if (msg_type == "audio") {
                std::shared_ptr<AudioMessageContent> text_msg_content_ptr = std::make_shared<AudioMessageContent>();
                text_msg_content_ptr->set_audio_path(content_query.value("info").toString().toStdString());
                message_ptr->set_msg_content(text_msg_content_ptr);
            } else if (msg_type == "image") {
                std::shared_ptr<ImageMessageContent> text_msg_content_ptr = std::make_shared<ImageMessageContent>();
                text_msg_content_ptr->set_image_path(content_query.value("info").toString().toStdString());
                message_ptr->set_msg_content(text_msg_content_ptr);
            }

            messages.push_back(message_ptr);
        } while (query.next());

        return messages;
    }
}

std::vector<std::string> LinkDovePSQLDataBase::get_chat_participants(unsigned long long group_id) {
    return get_channel_participants(group_id);
}

StatusInfo LinkDovePSQLDataBase::get_status_info(const std::string &username) {
    QSqlQuery query(data_base_);
    query.prepare("SELECT Cs.username, Cs.email, P.Status, P.avatar, P.birthday, Cr.name FROM Consumer Cs "
                  "INNER JOIN Profile P "
                  "ON P.consumer=Cs.id "
                  "INNER JOIN Country Cr "
                  "ON P.country = Cr.id "
                  "WHERE Cs.username=:username;");

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

StatusInfo LinkDovePSQLDataBase::get_status_info(unsigned long long id) {
    QSqlQuery query(data_base_);
    query.prepare("SELECT Cs.username, Cs.email, P.Status, P.avatar, P.birthday, Cr.name FROM Consumer Cs "
                  "INNER JOIN Profile P "
                  "ON P.consumer=Cs.id "
                  "INNER JOIN Country Cr "
                  "ON P.country = Cr.id "
                  "WHERE Cs.id=:id;");
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
        channel_info.id_        = query.value("id").toULongLong();
        channel_info.owner_id_  = query.value("owner").toULongLong();
        channel_info.name_      = query.value("name").toString().toStdString();
        channel_info.is_private_ = query.value("is_private").toBool();

        return channel_info;
    }

    ChatInfo retrieve_chat_info(const QSqlQuery &query) {
        ChatInfo chat_info;
        chat_info.id_        = query.value("id").toULongLong();
        chat_info.owner_id_  = query.value("owner").toULongLong();
        chat_info.name_      = query.value("name").toString().toStdString();
        chat_info.is_private_ = query.value("is_private").toBool();

        return chat_info;
    }

    StatusInfo retrieve_status_info(const QSqlQuery& query) {
        StatusInfo status_info;
        status_info.id_          = query.value("id").toULongLong();
        status_info.username_    = query.value("username").toString().toStdString();
        status_info.email_       = query.value("email").toString().toStdString();
        status_info.birthday_    = QDate::fromString(query.value("birthday").toString(), QString(BIRTHAY_FORMAT));
        status_info.text_status_ = query.value("text_status").toString().toStdString();
        status_info.image_path_  = query.value("avatar").toString().toStdString();
        status_info.is_banned_   = query.value("is_banned").toBool();

        return status_info;
    }

    std::vector<Complaint> retrieve_complaints(QSqlQuery& query, int count) {
        std::vector<Complaint> complaints;
        Complaint complaint;

        do {
            complaint.id_ = query.value("id").toInt();
            complaint.sender_id_ = query.value("sender").toInt();
            complaint.text_ = query.value("description").toString().toStdString();

            complaints.push_back(complaint);
            --count;

        } while (query.next() && count);

        return complaints;
    }

    std::vector<Notification> retrieve_notifications(QSqlQuery& query, int count) {
        std::vector<Notification> notifications;
        Notification notification;

        do {
            notification.id_ = query.value("id").toInt();
            notification.receiver_id_ = query.value("receiver").toInt();
            notification.text_ = query.value("description").toString().toStdString();

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
        unsigned long long sender_id = query.value("sender").toULongLong(),
                           receiver_id = query.value("receiver").toULongLong();

        do {
            std::shared_ptr<IndividualMessage> message_ptr = std::make_shared<IndividualMessage>();
            message_ptr->set_id(query.value("id").toULongLong());
            message_ptr->set_send_datetime(query.value("time").toDateTime());
            message_ptr->set_msg_edges(sender_id, receiver_id);

            msg_type = query.value("content_type").toString().toStdString();
            if (msg_type == "text") {
                std::shared_ptr<TextMessageContent> text_msg_content_ptr = std::make_shared<TextMessageContent>();
                text_msg_content_ptr->set_text(content_query.value("info").toString().toStdString());
                message_ptr->set_msg_content(text_msg_content_ptr);
            } else if (msg_type == "audio") {
                std::shared_ptr<AudioMessageContent> text_msg_content_ptr = std::make_shared<AudioMessageContent>();
                text_msg_content_ptr->set_audio_path(content_query.value("info").toString().toStdString());
                message_ptr->set_msg_content(text_msg_content_ptr);
            } else if (msg_type == "image") {
                std::shared_ptr<ImageMessageContent> text_msg_content_ptr = std::make_shared<ImageMessageContent>();
                text_msg_content_ptr->set_image_path(content_query.value("info").toString().toStdString());
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
        unsigned long long channel_id = query.value("id").toULongLong();

        do {
            std::shared_ptr<ChannelMessage> message_ptr = std::make_shared<ChannelMessage>();
            message_ptr->set_id(query.value("id").toULongLong());
            message_ptr->set_send_datetime(query.value("time").toDateTime());
            message_ptr->set_channel_id(channel_id);

            msg_type = query.value("content_type").toString().toStdString();
            if (msg_type == "text") {
                std::shared_ptr<TextMessageContent> text_msg_content_ptr = std::make_shared<TextMessageContent>();
                text_msg_content_ptr->set_text(content_query.value("info").toString().toStdString());
                message_ptr->set_msg_content(text_msg_content_ptr);
            } else if (msg_type == "audio") {
                std::shared_ptr<AudioMessageContent> text_msg_content_ptr = std::make_shared<AudioMessageContent>();
                text_msg_content_ptr->set_audio_path(content_query.value("info").toString().toStdString());
                message_ptr->set_msg_content(text_msg_content_ptr);
            } else if (msg_type == "image") {
                std::shared_ptr<ImageMessageContent> text_msg_content_ptr = std::make_shared<ImageMessageContent>();
                text_msg_content_ptr->set_image_path(content_query.value("info").toString().toStdString());
                message_ptr->set_msg_content(text_msg_content_ptr);
            }

            messages.push_back(message_ptr);
        } while (query.next());

        return messages;
    }
}
