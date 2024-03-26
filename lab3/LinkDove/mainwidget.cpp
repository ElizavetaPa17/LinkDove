#include "mainwidget.h"
#include "ui_mainwidget.h"

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

    if (strcmp(status_info.username_.c_str(), ADMIN_USERNAME) == 0) {
        setPrivilegedMode();
    } else {
        setSimpleMode();
    }
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

void MainWidget::slotQuit() {
    // clear status info value
    //memset(&status_info_, 0, sizeof(status_info_));

    ui->tabWidget->setCurrentIndex(EMPTY_PAGE);
    emit switchToPage(this, LOGIN_PAGE);
}

void MainWidget::setPrivilegedMode() {
    ui->complaintButton->setText("Жалобы");
    ui->editIconLabel->setDisabled(true);

    ui->privateButton->hide();
    ui->banButton->show();
}

void MainWidget::setSimpleMode() {
    ui->complaintButton->setText("Жалоба");
    ui->editIconLabel->setDisabled(false);

    ui->privateButton->show();
    ui->banButton->hide();
}

void MainWidget::setupConnection() {
    connect(ui->profileLabel, &ClickableLabel::clicked, this, &MainWidget::slotRedirectClick);
    connect(ui->chatLabel,    &ClickableLabel::clicked, this, &MainWidget::slotRedirectClick);
    connect(ui->settingLabel, &ClickableLabel::clicked, this, &MainWidget::slotRedirectClick);
    connect(ui->quitButton,   &QPushButton::clicked,    this, &MainWidget::slotQuit);
}
