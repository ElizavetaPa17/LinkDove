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

private:
    Ui::BanDialog *ui;
};

#endif // BANDIALOG_H
