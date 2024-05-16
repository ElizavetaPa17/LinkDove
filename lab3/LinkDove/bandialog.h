#ifndef BANDIALOG_H
#define BANDIALOG_H

#include <QDialog>
#include <utility>

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

    /**
     * <p> Возвращает никнейм пользователя для его блокировки/разблокировки. </p>
     * @brief get_info
     * @return - Пара <никнейм пользователя, флаг блокировки>
     */
    std::pair<QString, bool> get_info();

private slots:
    /**
     * <p> Слот, который делает кнопки блокировки/разблокировки доступными. </p>
     * @brief slotEnableButtons
     */
    void slotEnableButtons();

private:
    Ui::BanDialog *ui;
    bool is_ban_ = false;

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
