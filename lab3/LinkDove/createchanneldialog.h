#ifndef CREATECHANNELDIALOG_H
#define CREATECHANNELDIALOG_H

#include <QDialog>

namespace Ui {
class CreateChannelDialog;
}

class CreateChannelDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateChannelDialog(QWidget *parent = nullptr);
    ~CreateChannelDialog();

    /**
     * <p> Возвращает название создаваемого чата. </p>
     * @brief getChannelName
     * @return
     */
    std::string getChannelName();

private slots:
    /**
     * <p> Слот, который делает кнопку создания канала доступной. </p>
     * @brief slotEnableSendButton
     */
    void slotEnableCreateButton();

private:
    Ui::CreateChannelDialog *ui;

    /**
     * <p> Делает кнопку создания канала доступной/недоступной и меняет ее внешний вид. </p>
     * @brief setEnabledCreateButton
     * @param mode - Режим кнопки: доступна(true), недоступна(false)
     */
    void setEnabledCreateButton(bool mode);

    /**
     * <p> Настраивает соединения внутри виджета. </P>
     * @brief setupConnection
     */
    void setupConnection();
};

#endif // CREATECHANNELDIALOG_H
