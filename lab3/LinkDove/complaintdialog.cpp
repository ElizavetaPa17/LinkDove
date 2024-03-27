#include "complaintdialog.h"
#include "ui_complaintdialog.h"

#include <iostream>

ComplaintDialog::ComplaintDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ComplaintDialog)
{
    ui->setupUi(this);

    setEnabledSendButton(false);

    setupConnection();
}

ComplaintDialog::~ComplaintDialog()
{
    delete ui;
}

void ComplaintDialog::slotEnableSendButton() {
    if (ui->complaintEdit->text().size() > 0) {
        setEnabledSendButton(true);
    } else {
        setEnabledSendButton(false);
    }
}

void ComplaintDialog::setEnabledSendButton(bool mode) {
    if (mode) {
        ui->sendButton->setToolTip("");
        ui->sendButton->setEnabled(true);
    } else {
        ui->sendButton->setToolTip("Заполните поле описания.");
        ui->sendButton->setEnabled(false);
    }
}

void ComplaintDialog::setupConnection() {
    connect(ui->complaintEdit, &QLineEdit::textChanged, this, &ComplaintDialog::slotEnableSendButton);
}
