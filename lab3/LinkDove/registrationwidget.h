#ifndef REGISTRATIONWIDGET_H
#define REGISTRATIONWIDGET_H

#include <QWidget>
#include "UserInfo.h"

namespace Ui {
class RegistrationWidget;
}

/**
 * @brief The RegistrationWidget class
 * Виджет отображения формы для авторизации (форма регистрации)
 */
class RegistrationWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RegistrationWidget(QWidget *parent = nullptr);
    ~RegistrationWidget();

    /**
     * <p> Возвращает информацию, которая необходима для регистрации. </p>
     * @brief getRegistrationRequest
     * @return - Вектор строк, которые содержат значения полей формы регистрации.
     */
    UserInfo getRegistrationRequest();

    /**
     * <p> Очищает поля формы регистрации. </p>
     * @brief clearRegistrationInfo
     */
    void clearRegistrationInfo();

    /**
     *  При неудачной попытке регистрации отображает соответствующую информацию.
     * @brief displayFailedInfo
     */
    void displayFailedInfo(const QString& info);

signals:
    /**
     * <p> Отправляет сигнал после того, как пользователь пройдет авторизацию (форму регистрации).
     * Может переключаться либо на главную страницу MAIN_PAGE, либо на страницу логин-формы. </p>
     * @brief passRegistrationWidget
     * @param sender - Указатель на объект, который отправил сигнал.
     * @param page_index - Индекс страницы, на которую переключается приложение
     */
    void passRegistrationWidget(QWidget *sender, int page_index);

public slots:
    /**
     * <p> Проверяет корректность пользовательского ввода (все поля должны быть заполнены) </p>
     * @brief slotCheckInput
     */
    void slotCheckInput();

    /**
     * <p> Переключается на страницу логин-формы </p>
     * @brief slotSwitchToLogin
     */
    void slotSwitchToLogin();

private slots:
    /**
     * <p> Восстанавливает состояние ссылки на страницу логин-формы после неудачной попытки прохождения регистрации. </p>
     * @brief slotRestoreLoginLabel
     */
    void slotRestoreLoginLabel();

private:
    Ui::RegistrationWidget *ui;

    /**
     * <p> Настраивает соединения c виджетом авторизации (формы регистрации). </p>
     * @brief setupConnection
     */
    void setupConnection();
};

#endif // REGISTRATIONWIDGET_H
