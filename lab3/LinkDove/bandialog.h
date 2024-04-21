#ifndef BANDIALOG_H
#define BANDIALOG_H

#include <QDialog>

namespace Ui {
class BanDialog;
}

/**
 * @brief The BanDialog class
 * Виджет, отображающий администратору диалоговое окно с возможностью блокировки/разблокировки пользователя.
 */
class BanDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BanDialog(QWidget *parent = nullptr);
    ~BanDialog();

public slots:
    /**
     * <p> Слот, обрабатывающий результат блокировки/разблокировки пользователя. </p>
     * @brief slotHandleBanResult
     * @param result
     */
    void slotHandleBanResult(int result);

private slots:
    /**
     * <p> Слот, который делает кнопки блокировки/разблокировки доступными. </p>
     * @brief slotEnableButtons
     */
    void slotEnableButtons();

    /**
     * <p> Слот, обрабатывающий нажатие кнопки блокировки. </p>
     * @brief slotHandleBanButton
     */
    void slotHandleBanButton();

    /**
     * <p> Слот, обрабатывающий нажатие кнопки разблокировки. </p>
     * @brief slotHandleUnbanButton
     */
    void slotHandleUnbanButton();

private:
    Ui::BanDialog *ui;
    bool is_ban = false;

    /**
     * <p> Делает кнопки разблокировки доступными/недоступными и меняет их внешний вид. </p>
     * @brief setEnabledSendButton
     * @param mode - Режим кнопки: доступна(true), недоступна(false)
     */
    void setEnabledButtons(bool mode);

    /**
     * <p> Настраивает соединения внутри виджета. </P>
     * @brief setupConnection
     */
    void setupConnection();
};

#endif // BANDIALOG_H
