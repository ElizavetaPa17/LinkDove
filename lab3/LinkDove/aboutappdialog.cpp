#include "aboutappdialog.h"
#include "ui_aboutappdialog.h"

#include "constants.h"

AboutAppDialog::AboutAppDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutAppDialog)
{
    ui->setupUi(this);

    ui->descriptionLabel->setText(APPLICATION_DESCRIPTION);
    connect(ui->okButton, &QPushButton::clicked, this, &QDialog::accept);
}

AboutAppDialog::~AboutAppDialog()
{
    delete ui;
}
