#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QWidget>

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

signals:
    /**
     * <p> Отправляет сигнал после того, как пользователь пройдет авторизацию (логин-форму).
     * Может переключаться либо на главную страницу MAIN_PAGE, либо на страницу регистрации. </p>
     * @brief passLoginWidget
     * @param page_index - Индекс страницы, на которую переключается приложение
     */
    void passLoginWidget(int page_index);

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
    void slotSwitchToRegister();

private:
    Ui::LoginWidget *ui;

    /**
     * <p> Настраивает соединения c виджетом авторизации (логин-формы). </p>
     * @brief setupConnection
     */
    void setupConnection();
};

#endif // LOGINWIDGET_H
