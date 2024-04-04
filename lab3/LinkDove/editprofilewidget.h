#ifndef EDITPROFILEWIDGET_H
#define EDITPROFILEWIDGET_H

#include <QWidget>

#include "StatusInfo.h"

namespace Ui {
class EditProfileWidget;
}

/**
 * @brief The EditProfileWidget class
 * Виджет-страница, которая позволяет редактировать профиль пользователя.
 */
class EditProfileWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EditProfileWidget(QWidget *parent = nullptr);
    ~EditProfileWidget();

    /**
     * <p> Устанавливает информацию, которая будет редактироваться пользователем. </p>
     * @brief setStatusInfo
     * @param status_info - Структура, содержащая данные для редактирования
     */
    void setStatusInfo(const StatusInfo& status_info);

    /**
     * <p> Возвращает информацию о пользователе. </p>
     * @brief getStatusInfo
     * @return - Структура, содержащая информацию о пользователе.
     */
    StatusInfo getStatusInfo();

signals:
    /**
     * <p> Сигнал, который отправляется, когда пользователь закончил редактировать профиль. </p>
     * @brief editFinished
     * @param status_info - Структура, содержащая отредактированную информацию о пользователе.
     */
    void editFinished(StatusInfo status_info);

private slots:
    /**
     * <p> Слот, отвечающий за выбор картинки для пользователя. </p>
     * @brief slotChooseUserIcon
     */
    void slotChooseUserIcon();

    /**
     * <p> Слот, вызываемый при окончании редактирования профиля. </p>
     * @brief slotEditFinished
     */
    void slotEditFinished();

private:
    /**
     * <p> Настраивает соединения внутри виджета.</p>
     * @brief setupConnection
     */
    void setupConnection();

private:
    Ui::EditProfileWidget *ui;
};

#endif // EDITPROFILEWIDGET_H
