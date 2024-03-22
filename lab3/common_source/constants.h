#pragma once

enum Page {
    WELCOME_PAGE,
    LOGIN_PAGE,
    REGISTER_PAGE,
    MAIN_PAGE
};

// Минимальный возраст пользователя, разрешенный для создания аккаунта
const int MINIMUM_ALLOWED_AGE = 14;

// Адрес хоста, к которому подключается клиент
#define LOCAL_ADDRESS "127.0.0.1"

// Идентификатор конца запросов в операциях чтения-записи.
#define END_OF_REQUEST "f8377569513448db7baf7caa601d58730eeaf9af"

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
