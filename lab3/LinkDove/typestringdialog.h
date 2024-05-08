#ifndef TYPESTRINGDIALOG_H
#define TYPESTRINGDIALOG_H

#include <QDialog>

namespace Ui {
class TypeStringDialog;
}

class TypeStringDialog : public QDialog
{
    Q_OBJECT
public:
    explicit TypeStringDialog(QWidget *parent, const QString &text);
    ~TypeStringDialog();

    /**
     * <p> Возвращает строку, введенную пользователем. . </p>
     * @brief getString
     * @return
     */
    std::string getString();

private slots:
    /**
     * <p> Слот, который делает кнопку отправления жалобы доступной. </p>
     * @brief slotEnableSendButton
     */
    void slotEnableSendButton();

private:
    Ui::TypeStringDialog *ui;

    /**
     * <p> Делает кнопку "Готово" доступной/недоступной и меняет ее внешний вид. </p>
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

#endif // TYPESTRINGDIALOG_H
