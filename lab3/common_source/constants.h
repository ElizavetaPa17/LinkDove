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

    SEND_IND_MSG_SUCCESS_ANSWER,
    SEND_IND_MSG_FAILED_ANSWER,

    SEND_CHNNL_MSG_SUCCESS_ANSWER,
    SEND_CHNNL_MSG_FAILED_ANSWER,

    SEND_CHAT_MSG_SUCCESS_ANSWER,
    SEND_CHAT_MSG_FAILED_ANSWER,

    GET_IND_MSG_SUCCESS_ANSWER,
    GET_IND_MSG_FAILED_ANSWER,

    GET_INTERLOCUTORS_SUCCESS_ANSWER,
    GET_INTERLOCUTORS_FAILED_ANSWER,

    DELETE_IND_CHAT_SUCCESS_ANSWER,
    DELETE_IND_CHAT_FAILED_ANSWER,

    CREATE_CHANNEL_SUCCESS_ANSWER,
    CREATE_CHANNEL_FAILED_ANSWER,

    FIND_CHANNEL_SUCCESS_ANSWER,
    FIND_CHANNEL_FAILED_ANSWER,

    GET_CHANNELS_SUCCESS_ANSWER,
    GET_CHANNELS_FAILED_ANSWER,

    IS_CHANNEL_PARTICIPANT_SUCCESS_ANSWER,
    IS_CHANNEL_PARTICIPANT_FAILED_ANSWER,

    ADD_PARTICIPANT_TO_CHANNEL_SUCCESS_ANSWER,
    ADD_PARTICIPANT_TO_CHANNEL_FAILED_ANSWER,

    GET_CHNNL_MSG_SUCCESS_ANSWER,
    GET_CHNNL_MSG_FAILED_ANSWER,

    GET_CHNNL_PARTICIPANTS_SUCCESS_ANSWER,
    GET_CHNNL_PARTICIPANTS_FAILED_ANSWER,

    DELETE_CHANNEL_SUCCESS_ANSWER,
    DELETE_CHANNEL_FAILED_ANSWER,

    QUIT_CHANNEL_SUCCESS_ANSWER,
    QUIT_CHANNEL_FAILED_ANSWER,

    REMOVE_USER_FROM_CHANNEL_SUCCESS_ANSWER,
    REMOVE_USER_FROM_CHANNEL_FAILED_ANSWER,

    CREATE_CHAT_SUCCESS_ANSWER,
    CREATE_CHAT_FAILED_ANSWER,

    GET_CHATS_SUCCESS_ANSWER,
    GET_CHATS_FAILED_ANSWER,

    FIND_CHAT_SUCCESS_ANSWER,
    FIND_CHAT_FAILED_ANSWER,

    IS_CHAT_PARTICIPANT_SUCCESS_ANSWER,
    IS_CHAT_PARTICIPANT_FAILED_ANSWER,

    ADD_PARTICIPANT_TO_CHAT_SUCCESS_ANSWER,
    ADD_PARTICIPANT_TO_CHAT_FAILED_ANSWER,

    GET_CHAT_MSG_SUCCESS_ANSWER,
    GET_CHAT_MSG_FAILED_ANSWER,

    DELETE_CHAT_SUCCESS_ANSWER,
    DELETE_CHAT_FAILED_ANSWER,

    QUIT_CHAT_SUCCESS_ANSWER,
    QUIT_CHAT_FAILED_ANSWER,

    REMOVE_USER_FROM_CHAT_SUCCESS_ANSWER,
    REMOVE_USER_FROM_CHAT_FAILED_ANSWER,

    GET_CHAT_PARTICIPANTS_SUCCESS_ANSWER,
    GET_CHAT_PARTICIPANTS_FAILED_ANSWER,

    SEND_USER_ANSWER_SUCCESS_ANSWER,
    SEND_USER_ANSWER_FAILED_ANSWER,

    GET_NOTIFICATIONS_SUCCESS_ANSWER,
    GET_NOTIFICATIONS_FAILED_ANSWER,

    DEL_NOTIFICATION_SUCCESS_ANSWER,
    DEL_NOTIFICATION_FAILED_ANSWER,

    DEL_IND_MSG_SUCCESS_ANSWER,
    DEL_IND_MSG_FAILED_ANSWER,

    DEL_CHANNEL_MSG_SUCCESS_ANSWER,
    DEL_CHANNEL_MSG_FAILED_ANSWER,

    DEL_CHAT_MSG_SUCCESS_ANSWER,
    DEL_CHAT_MSG_FAILED_ANSWER,
};

enum MessageType {
    INDIVIDUAL_MSG_TYPE,
    CHANNEL_MSG_TYPE,
    GROUP_MSG_TYPE
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

// Ограничение на размер списка уведомлений, который отправляется пользователю
const int GET_NOTIFICATIONS_LIMIT = 100;

// Адрес хоста, к которому подключается клиент
#define LOCAL_ADDRESS "127.0.0.1"

// Идентификатор конца запросов в операциях чтения-записи.
#define END_OF_REQUEST "f8377569513448db7baf7caa601d58730eeaf9af"

// Формат строковой даты
#define BIRTHAY_FORMAT "yyyy-MM-dd"

// Формат строковой даты с отметкой времени дня
#define DATETIME_FORMAT "yyyy-MM-dd hh:mm:ss"

#define MEDIA_IND_IMAGE_PATH "media/ind_images/"
#define MEDIA_IND_AUDIO_PATH "media/ind_audio/"
#define MEDIA_CHANNEL_IMAGE_PATH "media/channel_images/"
#define MEDIA_CHANNEL_AUDIO_PATH "media/channel_audio/"
#define MEDIA_CHAT_IMAGE_PATH "media/chat_images/"
#define MEDIA_CHAT_AUDIO_PATH "media/chat_audio/"
#define MEDIA_AVATARS_IMAGE_PATH "media/avatars/"

// Идентификатор для идентификатора сущности, которая создана, но еще не инициализирована или не сохранена в БД.
const int CREATED_ENTITY_ID = 0;


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
// Ответ сервера на успешную отправку сообщения от пользователя в беседе между двумя участниками
extern const char *SEND_IND_MSG_SUCCESS;
// Ответ сервера на неудачную отправку сообщения от пользователя в беседе между двумя участниками
extern const char *SEND_IND_MSG_FAILED;
// Ответ сервера на успешную отправку сообщения в канал
extern const char *SEND_CHNNL_MSG_SUCCESS;
// Ответ сервера на неудачную отправку сообщения в канал
extern const char *SEND_CHNNL_MSG_FAILED;
// Ответ сервера на успешную отправку сообщения в чат (группу)
extern const char *SEND_CHAT_MSG_SUCCESS;
// Ответ сервера на неудачную отправку сообщения в чат (группу)
extern const char *SEND_CHAT_MSG_FAILED;

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

// Запрос на удаление переписки между двумя собеседниками
extern const char *DELETE_IND_CHAT_REQUEST;
// Ответ сервера на успешное удаление переписки
extern const char *DELETE_IND_CHAT_SUCCESS;
// Ответ сервера на неудачное удаление переписки
extern const char *DELETE_IND_CHAT_FAILED;

// Запрос на создание канала
extern const char *CREATE_CHANNEL_REQUEST;
// Ответ сервера на успешное создание канала
extern const char *CREATE_CHANNEL_SUCCESS;
// Ответ сервера на неудачное создание канала
extern const char *CREATE_CHANNEL_FAILED;

// Запрос на поиск информации о канале
extern const char *FIND_CHANNEL_REQUEST;
// Ответ сервера на успешный поиск информации о канале
extern const char *FIND_CHANNEL_SUCCESS;
// Ответ сервера на неудачный поиск информации о канале
extern const char *FIND_CHANNEL_FAILED;

// Запрос на получение информации о каналах, в которых состоит текущий пользователь
extern const char *GET_CHANNELS_REQUEST;
// Ответ сервера на успешную отправку информации о таких каналах
extern const char *GET_CHANNELS_SUCCESS;
// Ответ сервера на неудачную отправку информации о таких каналах
extern const char *GET_CHANNELS_FAILED;

// Запрос на получение информации о том, является ли пользователь участником канала
extern const char *IS_CHANNEL_PARTICIPANT_REQUEST;
// Ответ сервера на успешную отправку информации
extern const char *IS_CHANNEL_PARTICIPANT_SUCCESS;
// Ответ сервера на неудачную отправку информации
extern const char *IS_CHANNEL_PARTICIPANT_FAILED;

// Запрос на добавление текущего пользователя в канал
extern const char *ADD_PARTICIPANT_TO_CHANNEL_REQUEST;
// Ответ сервера на успешное добавление текущего пользователя в канал
extern const char *ADD_PARTICIPANT_TO_CHANNEL_SUCCESS;
// Ответ сервера на неудачное добавление текущего пользователя в канал
extern const char *ADD_PARTICIPANT_TO_CHANNEL_FAILED;

// Запрос на получение сообщений из канала
extern const char *GET_CHNNL_MSG_REQUEST;
// Ответ сервера на успешное получение сообщений из канала
extern const char *GET_CHNNL_MSG_SUCCESS;
// Ответ сервера на неудачное получение сообщений из канала
extern const char *GET_CHNNL_MSG_FAILED;

// Запрос на получение списка участников канала
extern const char *GET_CHNNL_PARTICIPANTS_REQUEST;
// Ответ сервера на успешное получение списка участников канала
extern const char *GET_CHNNL_PARTICIPANTS_SUCCESS;
// Ответ сервера на неудачное получение списка участников канала
extern const char *GET_CHNNL_PARTICIPANTS_FAILED;

// Запрос на удаление канала
extern const char *DELETE_CHANNEL_REQUEST;
// Ответ сервера на успешное удаление канала
extern const char *DELETE_CHANNEL_SUCCESS;
// Ответ сервера на неудачное удаление канала
extern const char *DELETE_CHANNEL_FAILED;

// Запрос на удаление из канала пользователя
extern const char *QUIT_CHANNEL_REQUEST;
// Ответ сервера на успешное удаление пользователя из канала
extern const char *QUIT_CHANNEL_SUCCESS;
// Ответ сервера на неудачное удаление пользователя из канала
extern const char *QUIT_CHANNEL_FAILED;

// Запрос на удаление пользователя из канала владельцем канала
extern const char *REMOVE_USER_FROM_CHANNEL_REQUEST;
// Ответ сервера на успешное удаление пользователя из канала владельцем канала
extern const char *REMOVE_USER_FROM_CHANNEL_SUCCESS;
// Ответ сервера на неудачное удаление пользователя из канала владельцем канала
extern const char *REMOVE_USER_FROM_CHANNEL_FAILED;

// Запрос на создание чата
extern const char *CREATE_CHAT_REQUEST;
// Ответ сервера на успешное создание чата
extern const char *CREATE_CHAT_SUCCESS;
// Ответ сервера на неудачное создание чата
extern const char *CREATE_CHAT_FAILED;

// Запрос на получение информации о чатах, в которых состоит текущий пользователь
extern const char *GET_CHATS_REQUEST;
// Ответ сервера на успешную отправку информации о таких чатах
extern const char *GET_CHATS_SUCCESS;
// Ответ сервера на неудачную отправку информации о таких чатах
extern const char *GET_CHATS_FAILED;

// Запрос на поиск информации о чате
extern const char *FIND_CHAT_REQUEST;
// Ответ сервера на успешный поиск информации о чате
extern const char *FIND_CHAT_SUCCESS;
// Ответ сервера на неудачный поиск информации о чате
extern const char *FIND_CHAT_FAILED;

// Запрос на получение информации о том, является ли пользователь участником чата
extern const char *IS_CHAT_PARTICIPANT_REQUEST;
// Ответ сервера на успешную отправку информации
extern const char *IS_CHAT_PARTICIPANT_SUCCESS;
// Ответ сервера на неудачную отправку информации
extern const char *IS_CHAT_PARTICIPANT_FAILED;

// Запрос на добавление текущего пользователя в чат
extern const char *ADD_PARTICIPANT_TO_CHAT_REQUEST;
// Ответ сервера на успешное добавление текущего пользователя в чат
extern const char *ADD_PARTICIPANT_TO_CHAT_SUCCESS;
// Ответ сервера на неудачное добавление текущего пользователя в чат
extern const char *ADD_PARTICIPANT_TO_CHAT_FAILED;

// Запрос на получение сообщений из чата
extern const char *GET_CHAT_MSG_REQUEST;
// Ответ сервера на успешное получение сообщений из чата
extern const char *GET_CHAT_MSG_SUCCESS;
// Ответ сервера на неудачное получение сообщений из чата
extern const char *GET_CHAT_MSG_FAILED;

// Запрос на получение списка участников чата (группы)
extern const char *GET_CHAT_PARTICIPANTS_REQUEST;
// Ответ сервера на успешное получение списка участников чата (группы)
extern const char *GET_CHAT_PARTICIPANTS_SUCCESS;
// Ответ сервера на неудачное получение списка участников чата (группы)
extern const char *GET_CHAT_PARTICIPANTS_FAILED;

// Запрос на удаление чата
extern const char *DELETE_CHAT_REQUEST;
// Ответ сервера на успешное удаление чата
extern const char *DELETE_CHAT_SUCCESS;
// Ответ сервера на неудачное удаление чата
extern const char *DELETE_CHAT_FAILED;

// Запрос на удаление из чата пользователя
extern const char *QUIT_CHAT_REQUEST;
// Ответ сервера на успешное удаление пользователя из чата
extern const char *QUIT_CHAT_SUCCESS;
// Ответ сервера на неудачное удаление пользователя из чата
extern const char *QUIT_CHAT_FAILED;

// Запрос на удаление пользователя из канала владельцем чата (группы)
extern const char *REMOVE_USER_FROM_CHAT_REQUEST;
// Ответ сервера на успешное удаление пользователя из канала владельцем чата (группы)
extern const char *REMOVE_USER_FROM_CHAT_SUCCESS;
// Ответ сервера на неудачное удаление пользователя из канала владельцем чата (группы)
extern const char *REMOVE_USER_FROM_CHAT_FAILED;

// Запрос на отправку ответа на жалобу пользователю
extern const char *SEND_USER_ANSWER_REQUEST;
// Ответ сервера на успешную отправку ответа на жалобу пользователя
extern const char *SEND_USER_ANSWER_SUCCESS;
// Ответ сервера на неудачную отправку ответа на жалобу пользователя
extern const char *SEND_USER_ANSWER_FAILED;

// Запрос на получение уведомлений от администратора
extern const char *GET_NOTIFICATIONS_REQUEST;
// Ответ сервера на успешное получение уведомлений
extern const char *GET_NOTIFICATIONS_SUCCESS;
// Ответ сервера на неудачное получение уведомлений
extern const char *GET_NOTIFICATIONS_FAILED;

// Запрос на удаление уведомления
extern const char *DEL_NOTIFICATION_REQUEST;
// Ответ сервера на успешное удаление уведомления
extern const char *DEL_NOTIFICATION_SUCCESS;
// Ответ сервера на неудачное удаление уведомления
extern const char *DEL_NOTIFICATION_FAILED;

// Запрос на удаление сообщения
extern const char *DEL_MSG_REQUEST;
// Ответ сервера на успешное удаление сообщения в инд. чате
extern const char *DEL_IND_MSG_SUCCESS;
// Ответ сервера на неудачное удаление сообщения в инд. чате
extern const char *DEL_IND_MSG_FAILED;
// Ответ сервера на успешное удаление сообщения в канале
extern const char *DEL_CHANNEL_MSG_SUCCESS;
// Ответ сервера на неудачное удаление сообщения в канале
extern const char *DEL_CHANNEL_MSG_FAILED;
// Ответ сервера на успешное удаление сообщения в чате (группе)
extern const char *DEL_CHAT_MSG_SUCCESS;
// Ответ сервера на неудачное удаление сообщения в чате (группе)
extern const char *DEL_CHAT_MSG_FAILED;
