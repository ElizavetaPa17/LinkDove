#include "complaintslistdialog.h"


#include <QVBoxLayout>
#include <QScrollArea>

#include "complaint.h"
#include "infodialog.h"
#include "complaintcard.h"
#include "clientsingleton.h"
#include "utility.h"

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

    connect(complaint_card, &ComplaintCard::removed, this, &ComplaintsListDialog::slotRemoveComplaint);
}

void ComplaintsListDialog::removeAllComplaints() {
    QLayout *pvboxLayout = scroll_area_->widget()->layout();
    QtUtility::clean_layout(pvboxLayout);
}

void ComplaintsListDialog::slotDeleteComplaintResult(int result) {
    std::string text;
    if (result == DEL_COMPLAINT_SUCCESS_ANSWER) {
        text = "Жалоба была успешно удалена.";
        removeComplaint(removed_complaint_id_);
    } else {
        text = "Ошибка удаления жалобы. Попытайтесь снова.";
    }

    std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, text);
    dialog_ptr->exec();
}

void ComplaintsListDialog::slotRemoveComplaint(unsigned long long id) {
    removed_complaint_id_ = id;
}

void ComplaintsListDialog::removeComplaint(unsigned long long complaint_id) {
    QLayout *layout = scroll_area_->widget()->layout();

    QLayoutItem *widget_item = nullptr;
    for (int i = 0; i < layout->count(); ++i) {
        if (((widget_item = layout->itemAt(i)) != nullptr) && widget_item->widget()) {
            if (static_cast<ComplaintCard*>(widget_item->widget())->getComplaintId() == complaint_id) {
                layout->removeItem(widget_item);
                delete widget_item->widget();
                delete widget_item;
                break;
            }
        }
    }
}
