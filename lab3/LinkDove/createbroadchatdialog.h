#ifndef CREATEBROADCHATDIALOG_H
#define CREATEBROADCHATDIALOG_H

#include <QDialog>
#include <utility>
#include <string>

namespace Ui {
class CreateBroadChatDialog;
}

// Диалоговое окно создания чата.
class CreateBroadChatDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateBroadChatDialog(QWidget *parent, const QString &text);
    ~CreateBroadChatDialog();

    /**
     * <p> Возвращает информацию, необходимую для создания чата. </p>
     * @brief getInfo
     * @return - Пара <название чата, флаг приватности"
     */
    std::pair<std::string, bool> getInfo();

private slots:
    /**
     * <p> Слот, который делает кнопку отправления жалобы доступной. </p>
     * @brief slotEnableSendButton
     */
    void slotEnableSendButton();

private:
    Ui::CreateBroadChatDialog *ui;

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

#endif // CREATEBROADCHATDIALOG_H
