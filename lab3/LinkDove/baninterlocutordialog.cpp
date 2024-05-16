#include "baninterlocutordialog.h"
#include "ui_baninterlocutordialog.h"

BanInterlocutorDialog::BanInterlocutorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BanInterlocutorDialog)
{
    ui->setupUi(this);

    setupConnection();
}

BanInterlocutorDialog::~BanInterlocutorDialog()
{
    delete ui;
}


bool BanInterlocutorDialog::get_info() {
    return is_ban_;
}

void BanInterlocutorDialog::setupConnection() {
    connect(ui->cancelButton, &QPushButton::clicked,   this, &QDialog::reject);
    connect(ui->banButton,    &QPushButton::clicked,   this, [this]() { is_ban_ = true; accept(); });
    connect(ui->unbanButton,  &QPushButton::clicked,   this, [this]() { is_ban_ = false; accept(); });
}
