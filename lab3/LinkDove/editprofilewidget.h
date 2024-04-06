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
     */
    void editFinished();

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
    StatusInfo status_info_;
};

#endif // EDITPROFILEWIDGET_H
