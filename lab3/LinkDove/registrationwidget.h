#ifndef REGISTRATIONWIDGET_H
#define REGISTRATIONWIDGET_H

#include <QWidget>

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

signals:
    /**
     * <p> Отправляет сигнал после того, как пользователь пройдет авторизацию (форму регистрации).
     * Может переключаться либо на главную страницу MAIN_PAGE, либо на страницу логин-формы. </p>
     * @brief passRegistrationWidget
     * @param page_index - Индекс страницы, на которую переключается приложение
     */
    void passRegistrationWidget(int page_index);

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

private:
    Ui::RegistrationWidget *ui;

    /**
     * <p> Настраивает соединения c виджетом авторизации (формы регистрации). </p>
     * @brief setupConnection
     */
    void setupConnection();
};

#endif // REGISTRATIONWIDGET_H
