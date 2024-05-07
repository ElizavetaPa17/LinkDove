#ifndef PROFILEWIDGET_H
#define PROFILEWIDGET_H

#include <QWidget>

#include "StatusInfo.h"

namespace Ui {
class ProfileWidget;
}

/**
 * @brief The ProfileWidget class
 * Виджет-страница, которая отображает профиль текущего пользователя или его собеседника.
 */
class ProfileWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ProfileWidget(QWidget *parent = nullptr);
    ~ProfileWidget();

    /**
     * <p> Устанавливает информацию о пользователе. </p>
     * @brief setStatusInfo
     */
    void setStatusInfo(const StatusInfo &status_info);

    /**
     * <p> Возвращает информацию о пользователе. </p>
     * @brief getStatusInfo
     * @return - Структура, содержащая информацию о пользователе.
     */
    StatusInfo getStatusInfo();

    /**
     * <p> Устанавливает привилегированный режим работы в зависимости от флага. </p>
     * @brief setPrivelegedMode
     * @param flag - Если flag = true, то режим привилегированный, иначе - обычный.
     */
    void setPrivelegedMode(bool flag);

    /**
     * <p> Выключает режим редактирования. Полезно при отображении профиля собеседника в классе InterlocutorProfileDialog. </p>
     * @brief disableEditingMode
     */
    void disableEditingMode();

signals:
    /**
     * <p> Сигнал, который отправляется,  когда пользователь хочет отредактировать профиль (идет переключение на виджет EditProfileWidget). </p>
     * @brief editProfile
     */
    void editProfile();

private:
    Ui::ProfileWidget *ui;
    StatusInfo status_info_;

    /**
     * <p> Настраивает соединения c виджетом авторизации (формы регистрации). </p>
     * @brief setupConnection
     */
    void setupConnection();
};

#endif // PROFILEWIDGET_H
