#include "createbroadchatdialog.h"
#include "ui_createbroadchatdialog.h"

CreateBroadChatDialog::CreateBroadChatDialog(QWidget *parent, const QString &text) :
    QDialog(parent),
    ui(new Ui::CreateBroadChatDialog)
{
    ui->setupUi(this);

    ui->descriptionLabel->setText(text);
    setEnabledSendButton(false);
    setupConnection();
}

CreateBroadChatDialog::~CreateBroadChatDialog()
{
    delete ui;
}

std::pair<std::string, bool> CreateBroadChatDialog::getInfo() {
    return std::pair(ui->complaintEdit->text().toStdString(), ui->privateButton->isChecked());
}

void CreateBroadChatDialog::slotEnableSendButton() {
    if (ui->complaintEdit->text().size() > 0) {
        setEnabledSendButton(true);
    } else {
        setEnabledSendButton(false);
    }
}

void CreateBroadChatDialog::setEnabledSendButton(bool mode) {
    if (mode) {
        ui->readyButton->setToolTip("");
        ui->readyButton->setEnabled(true);
    } else {
        ui->readyButton->setToolTip("Заполните поле описания.");
        ui->readyButton->setEnabled(false);
    }
}

void CreateBroadChatDialog::setupConnection() {
    connect(ui->complaintEdit,  &QLineEdit::textChanged, this, &CreateBroadChatDialog::slotEnableSendButton);
    connect(ui->cancelButton,   &QPushButton::clicked,   this, &QDialog::reject);
    connect(ui->readyButton,    &QPushButton::clicked,   this, &CreateBroadChatDialog::accept);
}
