#include "bandialog.h"
#include "ui_bandialog.h"

#include "clientsingleton.h"
#include "infodialog.h"

BanDialog::BanDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BanDialog)
{
    ui->setupUi(this);

    setEnabledButtons(false);
    setupConnection();
}

BanDialog::~BanDialog()
{
    delete ui;
}

std::pair<QString, bool> BanDialog::get_info() {
    return std::make_pair(ui->usernameEdit->text(), is_ban_);
}


void BanDialog::slotEnableButtons() {
    if (ui->usernameEdit->text().size() > 0) {
        setEnabledButtons(true);
    } else {
        setEnabledButtons(false);
    }
}

void BanDialog::setEnabledButtons(bool mode) {
    if (mode) {
        ui->banButton->setToolTip("");
        ui->banButton->setEnabled(true);
        ui->unbanButton->setToolTip("");
        ui->unbanButton->setEnabled(true);
    } else {
        ui->banButton->setToolTip("Заполните поле никнейма.");
        ui->banButton->setEnabled(false);
        ui->unbanButton->setToolTip("Заполните поле никнейма.");
        ui->unbanButton->setEnabled(false);
    }
}

void BanDialog::setupConnection() {
    connect(ui->usernameEdit, &QLineEdit::textChanged, this, &BanDialog::slotEnableButtons);
    connect(ui->cancelButton, &QPushButton::clicked,   this, &QDialog::reject);
    connect(ui->banButton,    &QPushButton::clicked,   this, [this]() { is_ban_ = true; accept(); });
    connect(ui->unbanButton,  &QPushButton::clicked,   this, [this]() { is_ban_ = false; accept(); });
}
