#ifndef SETTINGWIDGET_H
#define SETTINGWIDGET_H

#include <QWidget>

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
    void sendComplaint(std::string complaint_text);

public slots:
    /**
     * <p> Устанавливает привилегированный режим работы в зависимости от флага. </p>
     * @brief setPrivelegedMode
     * @param flag - Если flag = true, то режим привилегированный, иначе - обычный.
     */
    void setPrivilegedMode(bool flag);

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
     */
    void slotDisplayComplaintList();

    /**
     * <p> Отображает модальное окно с информацией о приложении. </p>
     * @brief slotDisplayAboutDialog
     */
    void slotDisplayAboutDialog();

private:
    Ui::SettingWidget *ui;
    int privileged_mode_;

    /**
     * <p> Настраивает соединения с классом. </p>
     * @brief setupConnections
     */
    void setupConnections();
};

#endif // SETTINGWIDGET_H