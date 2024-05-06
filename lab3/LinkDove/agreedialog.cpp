#include "agreedialog.h"
#include "ui_agreedialog.h"

AgreeDialog::AgreeDialog(QWidget *parent, const QString &text) :
    QDialog(parent),
    ui(new Ui::AgreeDialog)
{
    ui->setupUi(this);

    ui->label->setText(text);

    connect(ui->agreeButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(ui->cancelButton, &QPushButton::clicked,   this, &QDialog::reject);
}

AgreeDialog::~AgreeDialog()
{
    delete ui;
}
