#ifndef COMPLAINTDIALOG_H
#define COMPLAINTDIALOG_H

#include <QDialog>

namespace Ui {
class ComplaintDialog;
}

/**
 * @brief The ComplaintDialog class
 * Модальное диалоговое окно, которое позволяет пользователю написать жалобу админу.
 */
class ComplaintDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ComplaintDialog(QWidget *parent = nullptr);
    ~ComplaintDialog();

    /**
     * <p> Позволяет получить текст жалобы. </p>
     * @brief getComplaintText
     * @return - Текст жалобы.
     */
    std::string getComplaintText();

private slots:
    /**
     * <p> Слот, который делает кнопку отправления жалобы доступной. </p>
     * @brief slotEnableSendButton
     */
    void slotEnableSendButton();

private:
    Ui::ComplaintDialog *ui;

    /**
     * <p> Делает кнопку отправки жалобы доступной/недоступной и меняет ее внешний вид. </p>
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

#endif // COMPLAINTDIALOG_H
