#ifndef PROFILEWIDGET_H
#define PROFILEWIDGET_H

#include <QWidget>

#include "StatusInfo.h"

namespace Ui {
class ProfileWidget;
}

/**
 * Виджет-страница, которая отображает профиль пользователя.
 * @brief The ProfileWidget class
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

signals:
    /**
     * <p> Сигнал, который отправляется,  когда пользователь хочет отредактировать профиль (идет переключение на виджет EditProfileWidget). </p>
     * @brief editProfile
     */
    void editProfile();

private:
    Ui::ProfileWidget *ui;
    StatusInfo status_info_;
};

#endif // PROFILEWIDGET_H