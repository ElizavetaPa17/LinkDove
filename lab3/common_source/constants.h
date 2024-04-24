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

enum ResponseTypes {
    REGISTRATION_FAILED_ANSWER,
    REGISTRATION_SUCCESS_ANSWER,

    LOGIN_FAILED_ANSWER,
    LOGIN_BANNED_ANSWER,
    LOGIN_SUCCESS_ANSWER,

    SEND_COMPLAINT_SUCCESS_ANSWER,
    SEND_COMPLAINT_FAILED_ANSWER,

    DEL_COMPLAINT_SUCCESS_ANSWER,
    DEL_COMPLAINT_FAILED_ANSWER,

    GET_COMPLAINTS_SUCCESS_ANSWER,
    GET_COMPLAINTS_FAILED_ANSWER,

    UPDATE_USER_SUCCESS_ANSWER,
    UPDATE_USER_FAILED_ANSWER,

    FIND_USER_SUCCESS_ANSWER,
    FIND_USER_FAILED_ANWSER,

    BAN_USER_SUCCESS_ANSWER,
    BAN_USER_FAILED_ANSWER,

    SEND_MSG_SUCCESS_ANSWER,
    SEND_MSG_FAILED_ANSWER,

    GET_IND_MSG_SUCCESS_ANSWER,
    GET_IND_MSG_FAILED_ANSWER,

    GET_INTERLOCUTORS_SUCCESS_ANSWER,
    GET_INTERLOCUTORS_FAILED_ANSWER
};

enum MessageType {
    INDIVIDUAL_MSG_TYPE,
    BROAD_MSG_TYPE
};

enum MessageContentType {
    TEXT_MSG_TYPE,
    AUDIO_MSG_TYPE,
    IMAGE_MSG_TYPE
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

// Ограничение на размер списка жалоб, который отправляется администратору при запросе на его получение
const int GET_COMPLAINTS_LIMIT = 100;

// Адрес хоста, к которому подключается клиент
#define LOCAL_ADDRESS "127.0.0.1"

// Идентификатор конца запросов в операциях чтения-записи.
#define END_OF_REQUEST "f8377569513448db7baf7caa601d58730eeaf9af"

// Формат строковой даты
#define BIRTHAY_FORMAT "yyyy-MM-dd"

// Формат строковой даты с отметкой времени дня
#define DATETIME_FORMAT "yyyy-MM-dd hh:mm:ss"

// Идентификатор для сообщения, которое создано, но еще не инициализировано или не сохранено в БД.
const int CREATED_MSG_ID = 0;


// Информация о приложении LinkDove
extern const char *APPLICATION_DESCRIPTION;

// Типы сообщений
extern const char *TEXT_MSG;
extern const char *AUDIO_MSG;
extern const char *IMAGE_MSG;

/*
 * Типы запросов
*/

// Запрос клиента на авторизацию
extern const char *LOGIN_REQUEST;
// Ответ сервера в случае успешной авторизации
extern const char *LOGIN_SUCCESS;
// Ответ сервера в случае провала авторизации
extern const char *LOGIN_FAILED;
// Ответ сервера в случае, если пользователь
extern const char *LOGIN_BANNED;

// Запрос клиента на регистрацию
extern const char *REGISTER_REQUEST;
// Ответ сервера в случае успешной регистрации
extern const char *REGISTER_SUCCESS;
// Ответ сервера в случае провала регистрации
extern const char *REGISTER_FAILED;

// Запрос администратору, содержащий жалобу пользователя
extern const char *SEND_COMPLAINT_REQUEST;
// Ответ сервера на успешную регистрацию жалобы
extern const char *SEND_COMPLAINT_SUCCESS;
// Ответ сервера на неудачную регистрацию жалобы
extern const char *SEND_COMPLAINT_FAILED;

// Запрос на удаление жалобы от администратора
extern const char *DEL_COMPLAINT_REQUEST;
// Ответ сервера на успешное удаление жалобы
extern const char *DEL_COMPLAINT_SUCCESS;
// Ответ сервера на неудачное удаление жалобы
extern const char *DEL_COMPLAINT_FAILED;

// Запрос на получение жалоб
extern const char *GET_COMPLAINTS_REQUEST;
// Ответ сервера на успешный запрос жалоб
extern const char *GET_COMPLAINTS_SUCCESS;
// Ответ сервера на неудачный запрос жалоб
extern const char *GET_COMPLAINTS_FAILED;

// Запрос клиента на обновление информации о пользователе
extern const char *UPDATE_USER_REQUEST;
// Ответ сервера на успешное обновление информации о пользователе
extern const char *UPDATE_USER_SUCCESS;
// Ответ сервера на неудачное обновление информации о пользователе
extern const char *UPDATE_USER_FAILED;

// Запрос на поиск информации пользователя
extern const char *FIND_USER_REQUEST;
// Ответ сервера на успешный поиск информации о пользователе
extern const char *FIND_USER_SUCCESS;
// Ответ сервера на неудачный поиск информации о пользователе
extern const char *FIND_USER_FAILED;

// Запрос на блокирувку/разблокировку пользователя от администратора
extern const char *BAN_USER_REQUEST;
// Ответ сервера на успешную блокирувку/разблокировку пользователя
extern const char *BAN_USER_SUCCESS;
// Ответ сервера на неудачную блокирувку/разблокировку пользователя
extern const char *BAN_USER_FAILED;

// Запрос на отправку сообщения
extern const char *SEND_MSG_REQUEST;
// Ответ сервера на успешную отправку сообщения от пользователя
extern const char *SEND_MSG_SUCCESS;
// Ответ сервера на неудачную отправку сообщения от пользователя
extern const char *SEND_MSG_FAILED;

// Запрос на получение сообщений в чате с собеседником
extern const char *GET_IND_MSG_REQUEST;
// Ответ сервера на успешную отправку списка сообщений из чата с собеседником.
extern const char *GET_IND_MSG_SUCCESS;
// Ответ сервера на неудачную отправку списка сообщений из чата с собеседником.
extern const char *GET_IND_MSG_FAILED;

// Запрос на получение информации о собеседниках, с которыми у текущего пользователя есть активный чат
extern const char *GET_INTERLOCUTORS_REQUEST;
// Ответ сервера на успешную отправку информации о таких собеседниках
extern const char *GET_INTERLOCUTORS_SUCCESS;
// Ответ сервера на неудачную отправку информации о таких собеседниках
extern const char *GET_INTERLOCUTORS_FAILED;
