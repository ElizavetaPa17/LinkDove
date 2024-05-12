#include "complaintcard.h"
#include "ui_complaintcard.h"

#include <QString>
#include <QStyleOption>
#include <QPainter>

#include "complaint.h"
#include "infodialog.h"
#include "clientsingleton.h"
#include "typestringdialog.h"

ComplaintCard::ComplaintCard(QWidget *parent, const Complaint& complaint)
    : QWidget(parent)
    , ui(new Ui::ComplaintCard)
{
    ui->setupUi(this);

    QString text = "ID отправителя: ";
    text += QString::number(complaint.sender_id_);
    complaint_id_ = complaint.id_;

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

void ComplaintCard::slotAnswerUser() {
    std::unique_ptr<TypeStringDialog> dialog_ptr = std::make_unique<TypeStringDialog>(nullptr, "Введите описание ответа: ");
    if (dialog_ptr->exec() == QDialog::Accepted) {
        bool is_ok = false;
        unsigned long long user_id = ui->senderIdLabel->text().split(" ").last().toULongLong(&is_ok, 10);
        std::cerr << user_id << '\n';

        ClientSingleton::get_client()->async_answer_user(user_id, dialog_ptr->getString());
    }
}

void ComplaintCard::slotAnswerUserResult(int result) {
    if (result == SEND_USER_ANSWER_SUCCESS_ANSWER) {
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Ответ был успешно отправлен пользователю. ");
        dialog_ptr->exec();
    } else {
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Что-то пошло не так при отправлении ответа пользователю. ");
        dialog_ptr->exec();
    }
}

void ComplaintCard::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ComplaintCard::setupConnections() {
    connect(ClientSingleton::get_client(), &Client::answer_user_result, this, &ComplaintCard::slotAnswerUserResult);

    connect(ui->answerButton, &QPushButton::clicked, this, &ComplaintCard::slotAnswerUser);
    connect(ui->delButton,    &QPushButton::clicked, this, [this]() { ClientSingleton::get_client()->async_del_complaint(complaint_id_);
                                                                   emit removed(complaint_id_);
                                                                  });
}
