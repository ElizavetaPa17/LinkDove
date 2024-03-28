#ifndef PROFILEWIDGET_H
#define PROFILEWIDGET_H

#include <QWidget>

#include "StatusInfo.h"

namespace Ui {
class ProfileWidget;
}

/**
 * Виджет-страница, который отображает и позволяет редактировать профиль пользователя.
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
     * <p> Устанавливает привилегированный режим работы в зависимости от флага. </p>
     * @brief setPrivelegedMode
     * @param flag - Если flag = true, то режим привилегированный, иначе - обычный.
     */
    void setPrivelegedMode(bool flag);

private:
    Ui::ProfileWidget *ui;
};

#endif // PROFILEWIDGET_H
