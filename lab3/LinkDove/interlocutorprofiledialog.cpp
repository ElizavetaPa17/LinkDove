#include "interlocutorprofiledialog.h"
#include "ui_interlocutorprofiledialog.h"

#include <iostream>

InterlocutorProfileDialog::InterlocutorProfileDialog(QWidget *parent, const StatusInfo& interlocutor_info) :
    QDialog(parent),
    ui(new Ui::InterlocutorProfileDialog)
{
    ui->setupUi(this);

    std::cerr << interlocutor_info.username_ << '\n';
    ui->profileWidget->setStatusInfo(interlocutor_info);
    ui->profileWidget->disableEditingMode();
}

InterlocutorProfileDialog::~InterlocutorProfileDialog()
{
    delete ui;
}
