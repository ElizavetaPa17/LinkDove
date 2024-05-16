#ifndef BANINTERLOCUTORDIALOG_H
#define BANINTERLOCUTORDIALOG_H

#include <QDialog>

namespace Ui {
class BanInterlocutorDialog;
}

class BanInterlocutorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BanInterlocutorDialog(QWidget *parent = nullptr);
    ~BanInterlocutorDialog();

    /**
     * <p> Возвращает статус блокировки/разблокировки. </p>
     * @brief get_info
     * @return - Статус блокировки/разблокировки.
     */
    bool get_info();

private:
    Ui::BanInterlocutorDialog *ui;
    bool is_ban_ = false;

    /**
     * <p> Настраивает соединения внутри виджета. </P>
     * @brief setupConnection
     */
    void setupConnection();
};

#endif // BANINTERLOCUTORDIALOG_H
