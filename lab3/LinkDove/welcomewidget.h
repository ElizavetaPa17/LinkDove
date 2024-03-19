#ifndef WELCOMEWIDGET_H
#define WELCOMEWIDGET_H

#include <QWidget>
#include <QMouseEvent>

#include "transparentlabel.h"


/**
 * @brief The WelcomeWidget class
 * Виджет начальной страницы. Отображает картинку с логотипом приложения в течение
 * нескольких секунд, затем переходит к странице авторизации пользователя.
 */
class WelcomeWidget : public QWidget
{
    Q_OBJECT
public:
    WelcomeWidget(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

public slots:
    void slotEnableMousePress();

signals:
    /**
     * <p> Отправляет сигнал после того, как пользователь нажмет на экран после отработки анимации. </p>
     * @brief passWelcomePage
     * @param page_index - Индекс страницы, на которую переключается приложение
     */
    void passWelcomePage(int page_index);

protected:
    // Перегрузка функции для добавления возможности блокировки обработки
    // события нажатия кнопки мыши на экран, пока анимация не закончится.
    void mousePressEvent(QMouseEvent *event) override;

private:
    TransparentLabel *welcomeLabel_ = nullptr;
    bool is_press_blocked_ = true;
};

#endif // WELCOMEWIDGET_H
