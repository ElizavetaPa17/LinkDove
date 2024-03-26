#include "mainwidget.h"
#include "ui_mainwidget.h"

#include "constants.h"

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);

    ui->stackedWidget->setCurrentIndex(EMPTY_PAGE);
    setupConnection();
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::setStatusInfo(const StatusInfo& status_info) {
    status_info_ = status_info;

    ui->usernameLabel->setText(QString::fromUtf8(status_info.username_));
    ui->emailLabel->setText(QString::fromUtf8(status_info.email_));
    ui->birthdayLabel->setText(status_info.birthday_.toString(BIRTHAY_FORMAT));
    ui->textStatusLabel->setText(QString::fromUtf8(status_info.text_status_));
}

void MainWidget::slotRedirectClick(QWidget *sender) {
    if (sender == ui->profileLabel) {
        ui->stackedWidget->setCurrentIndex(PROFILE_PAGE);
    } else if (sender == ui->chatLabel) {
        ui->stackedWidget->setCurrentIndex(CHAT_PAGE);
    } else if (sender == ui->settingLabel) {
        ui->stackedWidget->setCurrentIndex(SETTING_PAGE);
    }
}

void MainWidget::setupConnection() {
    connect(ui->profileLabel, &ClickableLabel::clicked, this, &MainWidget::slotRedirectClick);
    connect(ui->chatLabel,    &ClickableLabel::clicked, this, &MainWidget::slotRedirectClick);
    connect(ui->settingLabel, &ClickableLabel::clicked, this, &MainWidget::slotRedirectClick);
}
