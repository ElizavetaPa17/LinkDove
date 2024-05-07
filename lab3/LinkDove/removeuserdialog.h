#ifndef REMOVEUSERDIALOG_H
#define REMOVEUSERDIALOG_H

#include <QDialog>

namespace Ui {
class RemoveUserDialog;
}

/**
 * @brief The RemoveUserDialog class
 * Виджет,
 */
class RemoveUserDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RemoveUserDialog(QWidget *parent = nullptr);
    ~RemoveUserDialog();

    /**
     * <p> Позволяет получить никнейм пользователя. </p>
     * @brief getComplaintText
     * @return - Никнейм пользователя.
     */
    std::string getUsername();

private slots:
    /**
     * <p> Слот, который делает кнопку отправления никнейма доступной. </p>
     * @brief slotEnableSendButton
     */
    void slotEnableSendButton();

private:
    Ui::RemoveUserDialog *ui;

    /**
     * <p> Делает кнопку отправки никнейма доступной/недоступной и меняет ее внешний вид. </p>
     * @brief setEnabledSendButton
     * @param mode - Режим кнопки: доступна(true), недоступна(false)
     */
    void setEnabledSendButton(bool mode);

    /**
     * <p> Настраивает соединения внутри виджета. </P>
     * @brief setupConnection
     */
    void setupConnection();
};

#endif // REMOVEUSERDIALOG_H
