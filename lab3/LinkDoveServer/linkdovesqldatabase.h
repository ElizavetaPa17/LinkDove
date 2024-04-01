#ifndef LINKDOVESQLDATABASE_H
#define LINKDOVESQLDATABASE_H

#include <QtSql/QSqlDatabase>

#include "UserInfo.h"
#include "logininfo.h"
#include "complaint.h"

class LinkDoveSQLDataBase
{
public:
    LinkDoveSQLDataBase(const std::string &connection_name);
    ~LinkDoveSQLDataBase();

    /**
     * <p> Добавляет пользователя в БД. </p>
     * @brief register_user
     * @param info - Информация о пользователе, который добавляется в БД.
     * @return - В случае успеха возвращает true, иначе - false.
     */
    bool register_user(const UserInfo& info);

    /**
     * <p> Проверяет, зарегистрирован ли пользователь в БД. </p>
     * @brief login_user
     * @param info - Информация пользователе, который проверяется на наличие в БД.
     * @return - В случае наличия пользователя в БД возвращает true, иначе - false.
     */
    bool login_user(const LoginInfo& info);

    /**
     * <p> Добавляет жалобу в БД. </p>
     * @brief add_complaint
     * @param complaint - Информация о жалобе, которая добавляется в БД.
     * @return - В случае успеха возвращает true, иначе - false.
     */
    bool add_complaint(const Complaint& complaint);

    /**
     * <p> Возвращает информацию о пользователе, который соответсвует заданным критериям. </p>
     * @brief get_user
     * @param username - Никнейм пользователя
     * @return - Структура, содержащая информацию о пользователе
     */
    StatusInfo get_status_info(const std::string &username);

private:
    QSqlDatabase data_base_;
    std::string connection_name_;

    /**
     * <p> Настраивает и устанавливает соединение с БД MySQL. </p>
     * @brief setup
     */
    void setup();

    /**
     * <p> Настраивает таблицы в БД. </p>
     * @brief setup_tables
     * @return - Возвращает true в случае успеха, иначе - false.
     */
    bool setup_tables();
};

#endif // LINKDOVESQLDATABASE_H
