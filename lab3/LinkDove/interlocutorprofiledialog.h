#ifndef INTERLOCUTORPROFILEDIALOG_H
#define INTERLOCUTORPROFILEDIALOG_H

#include <QDialog>
#include "StatusInfo.h"

namespace Ui {
class InterlocutorProfileDialog;
}

/**
 * @brief The InterlocutorProfileDialog class
 * Отображает информацию о собеседнике.
 */
class InterlocutorProfileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InterlocutorProfileDialog(QWidget *parent, const StatusInfo& interlocutor_info);
    ~InterlocutorProfileDialog();

private:
    Ui::InterlocutorProfileDialog *ui;
};

#endif // INTERLOCUTORPROFILEDIALOG_H
