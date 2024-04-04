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
     * <p> Обновляет данные о пользователе. </p>
     * @brief update_user
     * @param status_info - Структура, содержащая обновленную информацию о пользователе.
     * @return - В случае успешного обновления информации в БД возвращает true, иначе - false.
     */
    bool update_user(const StatusInfo& status_info);

    /**
     * <p> Добавляет жалобу в БД. </p>
     * @brief add_complaint
     * @param complaint - Информация о жалобе, которая добавляется в БД.
     * @return - В случае успеха возвращает true, иначе - false.
     */
    bool add_complaint(const Complaint& complaint);

    /**
     * <p> Возвращает информацию о пользователе, который соответствует никнейму. </p>
     * @brief get_user
     * @param username - Никнейм пользователя
     * @return - Структура, содержащая информацию о пользователе
     */
    StatusInfo get_status_info(const std::string &username);

    /**
     * <p> Возвращает информацию о пользователе, который соответствует идентификатору. </p>
     * @brief get_user
     * @param username - Никнейм пользователя
     * @return - Структура, содержащая информацию о пользователе
     */
    StatusInfo get_status_info(unsigned long long id);

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

    /**
     * <p> Извлекает данные о пользователе, которые возвращает объект query. </p>
     * @brief LinkDoveSQLDataBase::retrieve_status_info
     * @param query - Запрос, содержащий данные о пользователе.
     * @return - Структура. содержащая данные о пользователе.
     */
    StatusInfo retrieve_status_info(const QSqlQuery& query);
};

#endif // LINKDOVESQLDATABASE_H
