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

public slots:
    /**
     * <p> Устанавливает привилегированный режим работы в зависимости от флага. </p>
     * @brief setPrivelegedMode
     * @param flag - Если flag = true, то режим привилегированный, иначе - обычный.
     */
    void setPrivilegedMode(bool flag);

    /**
     * <p> Слот, отображающий модальное диалоговое окно для написания жалобы админу. </p>
     * @brief slotDisplayComplaintDialog
     */
    void slotDisplayComplaintDialog();

private slots:
    /**
     * <p> Принимает сигнал кнопки нажатия выхода и генерирует сигнал выхода. </p>
     * @brief slotQuitAccount
     */
    void slotQuitAccount();

private:
    Ui::SettingWidget *ui;

    /**
     * <p> Настраивает соединения с классом. </p>
     * @brief setupConnections
     */
    void setupConnections();
};

#endif // SETTINGWIDGET_H
