#pragma once

enum Page {
    WELCOME_PAGE,
    LOGIN_PAGE,
    REGISTER_PAGE,
    MAIN_PAGE,
};

enum InnerMainPage{
    EMPTY_PAGE,
    PROFILE_PAGE,
    CHAT_PAGE,
    SETTING_PAGE
};

enum profileTypePage {
    SIMPLE_PROFILE_PAGE,
    EDITED_PROFILE_PAGE
};

enum AutorizationAnswer {
    REGISTRATION_FAILED_ANSWER,
    REGISTRATION_SUCCESS_ANSWER,
    LOGIN_FAILED_ANSWER,
    LOGIN_SUCCESS_ANSWER,
    SEND_COMPLAINT_SUCCESS_ANSWER,
    SEND_COMPLAINT_FAILED_ANSWER
};

enum ApplicationMode {
    PRIVILEGED_MODE, // режим работы админа
    SIMPLE_MODE      // режим работы простого пользователя
};

// Минимальный возраст пользователя, разрешенный для создания аккаунта
const int MINIMUM_ALLOWED_AGE = 14;

// Значение невалидного идентификатора пользователя
const unsigned long long INVALID_USER_ID = 0;

// Идентификатор админа (суперпользователя)
const unsigned long long ADMIN_ID = 1;

// Адрес хоста, к которому подключается клиент
#define LOCAL_ADDRESS "127.0.0.1"

// Идентификатор конца запросов в операциях чтения-записи.
#define END_OF_REQUEST "f8377569513448db7baf7caa601d58730eeaf9af"

// Формат строковой даты
#define BIRTHAY_FORMAT "yyyy-MM-dd"

// Информация о приложении LinkDove
extern const char *APPLICATION_DESCRIPTION;

/*
 * Типы запросов
*/
// Запрос клиента на авторизацию
extern const char *LOGIN_REQUEST;
// Ответ сервера в случае успешной авторизации
extern const char *LOGIN_SUCCESS;
// Ответ сервера в случае провала авторизации
extern const char *LOGIN_FAILED;
// Запрос клиента на регистрацию
extern const char *REGISTER_REQUEST;
// Ответ сервера в случае успешной регистрации
extern const char *REGISTER_SUCCESS;
// Ответ сервера в случае провала регистрации
extern const char *REGISTER_FAILED;
// Запрос администратору, содержащий жалобу пользователя
extern const char *COMPLAINT_REQUEST;
// Ответ сервера на успешную регистрацию жалобы
extern const char *COMPLAINT_SUCCESS;
// Ответ сервера на неудачную регистрацию жалобы
extern const char *COMPLAINT_FAILED;
