#include "complaintcard.h"
#include "ui_complaintcard.h"

#include <QString>
#include <QStyleOption>
#include <QPainter>

#include "complaint.h"
#include "infodialog.h"
#include "clientsingleton.h"

ComplaintCard::ComplaintCard(QWidget *parent, const Complaint& complaint) :
    QWidget(parent),
    ui(new Ui::ComplaintCard)
{
    ui->setupUi(this);

    QString text = "ID отправителя: ";
    text += QString::number(complaint.id_);
    complaint_id_ = complaint.sender_id_;

    ui->senderIdLabel->setText(text);
    ui->textLabel->setText(complaint.text_.c_str());

    setupConnections();
}

ComplaintCard::~ComplaintCard()
{
    delete ui;
}

unsigned long long ComplaintCard::getComplaintId() {
    return complaint_id_;
}

void ComplaintCard::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ComplaintCard::setupConnections() {
    connect(ui->delButton, &QPushButton::clicked, this, [this]() { ClientSingleton::get_client()->async_del_complaint(complaint_id_); });
}
