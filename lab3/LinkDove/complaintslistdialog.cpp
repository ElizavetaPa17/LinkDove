#include "complaintslistdialog.h"


#include <QVBoxLayout>
#include <QScrollArea>

#include "complaint.h"
#include "infodialog.h"
#include "complaintcard.h"
#include "clientsingleton.h"

#include <iostream>
#include <QListWidget>

ComplaintsListDialog::ComplaintsListDialog(QWidget *parent)
    : QDialog(parent)
{
    scroll_area_ = new QScrollArea(this);

    QVBoxLayout *pvboxLayout = new QVBoxLayout;
    setLayout(pvboxLayout);
    layout()->addWidget(scroll_area_);

    pvboxLayout = new QVBoxLayout;
    pvboxLayout->setSpacing(5);
    scroll_area_->setWidget(new QWidget());
    scroll_area_->widget()->setLayout(pvboxLayout);
    scroll_area_->setWidgetResizable(true);

    setFixedHeight(600);
    setFixedWidth(400);
    setStyleSheet("background-color: rgb(245, 245, 245); \
                   border-radius: 51;");

    connect(ClientSingleton::get_client(), &Client::del_complaint_result, this, &ComplaintsListDialog::slotDeleteComplaintResult);
}

ComplaintsListDialog::~ComplaintsListDialog() {
}

void ComplaintsListDialog::addComplaint(const Complaint& complaint) {
    ComplaintCard *complaint_card = new ComplaintCard(this, complaint);
    scroll_area_->widget()->layout()->addWidget(complaint_card);
}

void ComplaintsListDialog::removeComplaint(unsigned long long complaint_id) {
    QLayout *pvboxLayout = scroll_area_->widget()->layout();
    size_t count = pvboxLayout->count();
    QWidget *widget = nullptr;

    for (int i = 0; i < count; ++i) {
        widget = pvboxLayout->itemAt(i)->widget();
        if (widget != nullptr) {
            if (static_cast<ComplaintCard*>(widget)->getComplaintId() == complaint_id) {
                scroll_area_->layout()->removeWidget(widget);
                break;
            }
        }
    }
}

void ComplaintsListDialog::removeAllComplaints() {
    QLayout *pvboxLayout = scroll_area_->widget()->layout();
    size_t count = pvboxLayout->count();
    QLayoutItem *widget_item = nullptr;

    while (widget_item = (pvboxLayout->takeAt(0))) {
        if (widget_item != nullptr) {
            delete widget_item;
        }
    }
}

void ComplaintsListDialog::slotDeleteComplaintResult(int result) {
    QString text;
    if (result == DEL_COMPLAINT_SUCCESS_ANSWER) {
        text = "Жалоба была успешно удалена.";
    } else {
        text = "Ошибка удаления жалобы. Попытайтесь снова.";
    }

    std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(text.toStdString());
    dialog_ptr->exec();
}
