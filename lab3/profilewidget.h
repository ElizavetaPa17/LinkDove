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
    ~ProfilePage();

    /**
     * <p> Устанавливает информацию о пользователе. </p>
     * @brief setStatusInfo
     */
    void setStatusInfo(const StatusInfo &status_info);

private:
    Ui::ProfileWidget *ui;
};

#endif // PROFILEWIDGET_H
