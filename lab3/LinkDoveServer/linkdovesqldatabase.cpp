#include "linkdovesqldatabase.h"

#include <iostream>
#include <QSqlError>
#include <QSqlQuery>

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
                            "  username VARCHAR(40) UNIQUE, "
                            "  email VARCHAR(256) UNIQUE, "
                            "  birthday DATE NOT NULL, "
                            "  text_status VARCHAR(256), "
                            "  image MEDIUMBLOB, "
                            "  is_banned TINYINT DEFAULT 0) ");

    if (!is_ok) {
        std::cerr << "Failed to setup USERS table: " << query.lastError().text().toStdString() << '\n';
        return false;
    } else {
        return true;
    }
}

bool LinkDoveSQLDataBase::register_user(UserInfo info) {
    QSqlQuery query(data_base_);
    query.prepare("INSERT INTO USERS (username, email, birthday, text_status, image, is_banned) "
                  "VALUES (:username, :email, :birthday, :text_status, :image, :is_banned)");

    query.bindValue(":username",    info.status_info_.username_.c_str());
    query.bindValue(":email",       info.status_info_.email_.c_str());
    query.bindValue(":birthday",    info.status_info_.birthday_.toString("yyyy-MM-dd"));
    query.bindValue(":text_status", info.status_info_.text_status_.c_str());
    query.bindValue(":is_banned",   0); // при создании пользователя его аккаунт не блокируется

    std::cerr << '\n' << info.status_info_.birthday_.isValid() << std::endl;

    if (query.exec()) {
        std::cerr << query.lastError().text().toStdString();
        return false;
    } else {
        std::cerr << query.lastError().text().toStdString();
        return true;
    }
}
