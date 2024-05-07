#include "removeuserdialog.h"
#include "ui_removeuserdialog.h"

RemoveUserDialog::RemoveUserDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RemoveUserDialog)
{
    ui->setupUi(this);

    setEnabledSendButton(false);
    setupConnection();
}

RemoveUserDialog::~RemoveUserDialog()
{
    delete ui;
}

std::string RemoveUserDialog::getUsername() {
    return ui->usernameEdit->text().toStdString();
}

void RemoveUserDialog::slotEnableSendButton() {
    if (ui->usernameEdit->text().size() > 0) {
        setEnabledSendButton(true);
    } else {
        setEnabledSendButton(false);
    }
}


void RemoveUserDialog::setEnabledSendButton(bool mode) {
    if (mode) {
        ui->sendButton->setToolTip("");
        ui->sendButton->setEnabled(true);
    } else {
        ui->sendButton->setToolTip("Заполните поле описания.");
        ui->sendButton->setEnabled(false);
    }
}

void RemoveUserDialog::setupConnection() {
    connect(ui->usernameEdit, &QLineEdit::textChanged, this, &RemoveUserDialog::slotEnableSendButton);
    connect(ui->cancelButton,  &QPushButton::clicked,   this, &QDialog::reject);
    connect(ui->sendButton,    &QPushButton::clicked,   this, &RemoveUserDialog::accept);
}
