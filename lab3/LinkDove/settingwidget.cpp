#include "settingwidget.h"
#include "ui_settingwidget.h"

#include "complaintdialog.h"

SettingWidget::SettingWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingWidget)
{
    ui->setupUi(this);

    setupConnections();
}

SettingWidget::~SettingWidget()
{
    delete ui;
}

void SettingWidget::setPrivilegedMode(bool flag) {
    if (flag) {
        ui->complaintButton->setText("Жалоба");

        ui->privateButton->show();
        ui->banButton->hide();
    } else {
        ui->complaintButton->setText("Жалобы");

        ui->privateButton->hide();
        ui->banButton->show();
    }
}

void SettingWidget::slotQuitAccount() {
    emit quitAccount();
}

void SettingWidget::slotDisplayComplaintDialog() {
    std::unique_ptr<ComplaintDialog> dialog_ptr = std::make_unique<ComplaintDialog>();
    if (dialog_ptr->exec() == QDialog::Accepted) {
        //
    }
}

void SettingWidget::setupConnections() {
    connect(ui->quitButton, &QPushButton::clicked, this, &SettingWidget::slotQuitAccount);
    connect(ui->complaintButton, &QPushButton::clicked,    this, &SettingWidget::slotDisplayComplaintDialog);
}
