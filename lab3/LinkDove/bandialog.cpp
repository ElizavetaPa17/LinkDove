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

void BanDialog::slotHandleBanResult(int result) {
    std::string text;
    if (result == BAN_USER_SUCCESS_ANSWER) {
        text = is_ban ? "Пользователь успешно заблокирован." : "Пользователь успешно разблокирован.";
    } else {
        text = "Ошибка. Попытайтесь снова.";
    }

    std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(text);
    dialog_ptr->exec();
}

void BanDialog::slotEnableButtons() {
    if (ui->usernameEdit->text().size() > 0) {
        setEnabledButtons(true);
    } else {
        setEnabledButtons(false);
    }
}

void BanDialog::slotHandleBanButton() {
    ClientSingleton::get_client()->async_ban_user(ui->usernameEdit->text().toStdString(), true);
    is_ban = true;
}

void BanDialog::slotHandleUnbanButton() {
    ClientSingleton::get_client()->async_ban_user(ui->usernameEdit->text().toStdString(), false);
    is_ban = false;
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
    connect(ui->banButton,    &QPushButton::clicked,   this, &BanDialog::slotHandleBanButton);
    connect(ui->unbanButton,  &QPushButton::clicked,   this, &BanDialog::slotHandleUnbanButton);
}
