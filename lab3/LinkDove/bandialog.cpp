#include "bandialog.h"
#include "ui_bandialog.h"

BanDialog::BanDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BanDialog)
{
    ui->setupUi(this);
}

BanDialog::~BanDialog()
{
    delete ui;
}
