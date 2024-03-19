#include "loginwidget.h"

#include <QLineEdit>
#include <QVBoxLayout>

LoginWidget::LoginWidget(QWidget *parent, Qt::WindowFlags flags)
    : QWidget(parent, flags)
{
    usernameEdit_ = new QLineEdit(this);
    emailEdit_    = new QLineEdit(this);
    passwordEdit_ = new QLineEdit(this);

    QVBoxLayout *pvboxLayout = new QVBoxLayout(this);
    pvboxLayout->addWidget(usernameEdit_);
    pvboxLayout->addWidget(emailEdit_);
    pvboxLayout->addWidget(passwordEdit_);
    setLayout(pvboxLayout);

    setupDesign();
}

void LoginWidget::setupDesign() {
    setupTextEditDesign(usernameEdit_);
    setupTextEditDesign(emailEdit_);
    setupTextEditDesign(passwordEdit_);


}

void LoginWidget::setupTextEditDesign(QLineEdit *lineEdit) {
    lineEdit->setStyleSheet("background-color: rgb(255, 255, 255);"
                            "width: 200px;"
                            "height: 20px;");
}
