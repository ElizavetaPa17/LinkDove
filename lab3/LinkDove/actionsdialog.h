#ifndef ACTIONSDIALOG_H
#define ACTIONSDIALOG_H

#include <QDialog>
#include <vector>
#include "action.h"

namespace Ui {
class ActionsDialog;
}

class ActionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ActionsDialog(QWidget *parent, std::vector<Action> &actions);
    ~ActionsDialog();

private:
    Ui::ActionsDialog *ui;
};

#endif // ACTIONSDIALOG_H
