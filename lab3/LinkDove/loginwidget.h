#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QWidget>
#include "logininfo.h"

namespace Ui {
class LoginWidget;
}

/**
 * @brief The LoginWidget class
 * Виджет отображения формы для авторизации (логин-форма)
 */
class LoginWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LoginWidget(QWidget *parent = nullptr);
    ~LoginWidget();

    /**
     * <p> Возвращает информацию, которая необходима для авторизации. </p>
     * @brief getLoginRequest
     * @return - Вектор строк, которые содержат значения полей логин-формы.
     */
    LoginInfo getLoginRequest();

    /**
     * <p> Очищает поля логин-формы. </p>
     * @brief clearLoginInfo
     */
    void clearLoginInfo();

    /**
     * <p> При неудачной попытке авторизации отображает соответствующую информацию. </p>
     * @brief displayFailedInfo
     */
    void displayFailedInfo(const QString& info);

signals:
    /**
     * <p> Отправляет сигнал после того, как пользователь пройдет авторизацию (логин-форму).
     * Может переключаться либо на главную страницу MAIN_PAGE, либо на страницу регистрации. </p>
     * @brief passLoginWidget
     * @param sender - Указатель на объект, который отправил сигнал.
     * @param page_index - Индекс страницы, на которую переключается приложение
     */
    void passLoginWidget(QWidget *sender, int page_index);

public slots:
    /**
     * <p> Проверяет корректность пользовательского ввода (все поля должны быть заполнены) </p>
     * @brief slotCheckInput
     */
    void slotCheckInput();

    /**
     * <p> Переключается на страницу регистрации </p>
     * @brief slotSwitchToRegister
     */
    //void slotSwitchToRegister();

private slots:
    /**
     * <p> Восстанавливает состояние ссылки на страницу регистрации после неудачной попытки прохождения логин-формы. </p>
     * @brief slotRestoreLoginLabel
     */
    void slotRestoreRegistrationLabel();

private:
    Ui::LoginWidget *ui;

    /**
     * <p> Настраивает соединения c виджетом авторизации (логин-формы). </p>
     * @brief setupConnection
     */
    void setupConnection();
};

#endif // LOGINWIDGET_H
