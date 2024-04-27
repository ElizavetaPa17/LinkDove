#include "createchanneldialog.h"
#include "ui_createchanneldialog.h"

CreateChannelDialog::CreateChannelDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateChannelDialog)
{
    ui->setupUi(this);

    setupConnection();
}

CreateChannelDialog::~CreateChannelDialog()
{
    delete ui;
}

std::string CreateChannelDialog::getChannelName() {
    return ui->nameEdit->text().toStdString();
}

void CreateChannelDialog::slotEnableCreateButton() {
    if (ui->nameEdit->text().size() > 0) {
        setEnabledCreateButton(true);
    } else {
        setEnabledCreateButton(false);
    }
}

void CreateChannelDialog::setEnabledCreateButton(bool mode) {
    if (mode) {
        ui->createButton->setToolTip("");
        ui->createButton->setEnabled(true);
    } else {
        ui->createButton->setToolTip("Заполните поле названия.");
        ui->createButton->setEnabled(false);
    }
}

void CreateChannelDialog::setupConnection() {
    connect(ui->nameEdit, &QLineEdit::textChanged, this, &CreateChannelDialog::slotEnableCreateButton);
    connect(ui->cancelButton,  &QPushButton::clicked,   this, &QDialog::reject);
    connect(ui->createButton,    &QPushButton::clicked,   this, &CreateChannelDialog::accept);
}
