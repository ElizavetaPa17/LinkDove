#include "typestringdialog.h"
#include "ui_typestringdialog.h"

TypeStringDialog::TypeStringDialog(QWidget *parent, const QString &text) :
    QDialog(parent),
    ui(new Ui::TypeStringDialog)
{
    ui->setupUi(this);

    ui->descriptionLabel->setText(text);
    setEnabledSendButton(false);
    setupConnection();
}

TypeStringDialog::~TypeStringDialog()
{
    delete ui;
}


std::string TypeStringDialog::getString() {
    return ui->complaintEdit->text().toStdString();
}


void TypeStringDialog::slotEnableSendButton() {
    if (ui->complaintEdit->text().size() > 0) {
        setEnabledSendButton(true);
    } else {
        setEnabledSendButton(false);
    }
}

void TypeStringDialog::setEnabledSendButton(bool mode) {
    if (mode) {
        ui->readyButton->setToolTip("");
        ui->readyButton->setEnabled(true);
    } else {
        ui->readyButton->setToolTip("Заполните поле описания.");
        ui->readyButton->setEnabled(false);
    }
}

void TypeStringDialog::setupConnection() {
    connect(ui->complaintEdit,  &QLineEdit::textChanged, this, &TypeStringDialog::slotEnableSendButton);
    connect(ui->cancelButton,   &QPushButton::clicked,   this, &QDialog::reject);
    connect(ui->readyButton,    &QPushButton::clicked,   this, &TypeStringDialog::accept);
}
