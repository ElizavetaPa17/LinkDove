#ifndef INFODIALOG_H
#define INFODIALOG_H

#include <QDialog>

namespace Ui {
class InfoDialog;
}

/**
 * @brief The InfoDialog class
 * Модальное диалоговое окно, отобращающее информационный текст пользователю.
 */
class InfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InfoDialog(const std::string &text, QWidget *parent = nullptr);
    ~InfoDialog();

private:
    Ui::InfoDialog *ui;
};

#endif // INFODIALOG_H
