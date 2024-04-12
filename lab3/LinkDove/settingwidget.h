#ifndef SETTINGWIDGET_H
#define SETTINGWIDGET_H

#include <QWidget>

#include "complaintslistdialog.h"

namespace Ui {
class SettingWidget;
}

/**
 * @brief The SettingWidget class
 * Класс, отвечающий за настройки и дополнительный функционал приложения.
 */
class SettingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SettingWidget(QWidget *parent = nullptr);
    ~SettingWidget();    

signals:
    /**
     * <p> Генерирует сигнал выхода из аккаунта. </p>
     * @brief quitAccount
     */
    void quitAccount();

    /**
     * <p> Генерирует сигнал для отправки жалобы. </p>
     * @brief sendComplaint
     * @param complaint_text - Содержание жалобы.
     */
    //void sendComplaint(std::string complaint_text);

public slots:
    /**
     * <p> Устанавливает привилегированный режим работы в зависимости от флага. </p>
     * @brief setPrivelegedMode
     * @param flag - Если flag = true, то режим привилегированный, иначе - обычный.
     */
    void setPrivilegedMode(bool flag);

    /**
     * <p> Слот, принимающий от клиента результат отправки жалобы. </p>
     * @brief slotComplaintResult
     * @param complaint_result - Результат отправки жалобы.
     */
    void slotComplaintResult(int complaint_result);

private slots:
    /**
     * <p> Принимает сигнал кнопки нажатия выхода и генерирует сигнал выхода. </p>
     * @brief slotQuitAccount
     */
    void slotQuitAccount();

    /**
     * <p> Слот, отображающий модальное диалоговое окно для написания жалобы админу. </p>
     * @brief slotDisplayComplaintDialog
     */
    void slotDisplayComplaintDialog();

    /**
     * <p> Слот, отображающий список жалоб админу. </p>
     * @brief slotDisplayComplaintList
     * @param get_complaints_result - Результат получения списка жалоб.
     */
    void slotDisplayComplaintList(int complaints_result);

    /**
     * <p> Отображает модальное окно с информацией о приложении. </p>
     * @brief slotDisplayAboutDialog
     */
    void slotDisplayAboutDialog();

private:
    Ui::SettingWidget *ui;
    ComplaintsListDialog complaint_dialog_;

    int privileged_mode_;

    /**
     * <p> Настраивает соединения с классом. </p>
     * @brief setupConnections
     */
    void setupConnections();
};

#endif // SETTINGWIDGET_H
