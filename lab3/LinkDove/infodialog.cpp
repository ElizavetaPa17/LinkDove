#include "infodialog.h"
#include "ui_infodialog.h"

InfoDialog::InfoDialog(const std::string& text, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InfoDialog)
{
    ui->setupUi(this);

    ui->descriptionLabel->setText(text.c_str());
    connect(ui->okButton, &QPushButton::clicked, this, &QDialog::accept);
}

InfoDialog::~InfoDialog()
{
    delete ui;
}