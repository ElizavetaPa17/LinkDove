#ifndef INTERLOCUTORPROFILEDIALOG_H
#define INTERLOCUTORPROFILEDIALOG_H

#include <QDialog>
#include "StatusInfo.h"

namespace Ui {
class InterlocutorProfileDialog;
}

/**
 * <p> Отображает информацию о собеседнике. </p>
 * @brief The InterlocutorProfileDialog class
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
