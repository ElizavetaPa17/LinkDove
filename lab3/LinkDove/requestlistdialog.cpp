#include "requestlistdialog.h"

#include <QScrollArea>

#include "requestcard.h"
#include "utility.h"
#include "clientsingleton.h"
#include "infodialog.h"

RequestListDialog::RequestListDialog(QWidget *parent)
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

    setupConnections();
}

RequestListDialog::~RequestListDialog() {

}

void RequestListDialog::setBroadChatInfo(unsigned long long broad_chat_id, bool is_channel) {
    broad_chat_id_ = broad_chat_id;
    is_channel_ = is_channel;
}

void RequestListDialog::addRequestUsername(const std::string &username) {
    RequestCard *request_card = new RequestCard(this, username);

    scroll_area_->widget()->layout()->addWidget(request_card);
    connect(request_card, &RequestCard::add, this, &RequestListDialog::slotAddParticipant);
    connect(request_card, &RequestCard::remove, this, &RequestListDialog::slotRemoveRequest);
}

void RequestListDialog::removeAllRequests() {
    QLayout *pvboxLayout = scroll_area_->widget()->layout();
    QtUtility::clean_layout(pvboxLayout);
}

void RequestListDialog::slotRemoveRequest(std::string username) {
    removed_request_username_ = username;

    if (is_channel_) {
        ClientSingleton::get_client()->async_delete_request_channel_request(username, broad_chat_id_);
    } else {
        ClientSingleton::get_client()->async_delete_request_chat_request(username, broad_chat_id_);
    }
}

void RequestListDialog::slotAddParticipant(std::string username) {
    removed_request_username_ = username;

    if (is_channel_) {
        ClientSingleton::get_client()->async_add_private_channel_participant_request(username, broad_chat_id_);
    } else {
        ClientSingleton::get_client()->async_add_private_chat_participant_request(username, broad_chat_id_);
    }
}

void RequestListDialog::slotRemoveRequestResult(int result) {
    if (result == REMOVE_REQUEST_CHANNEL_SUCCESS_ANSWER) {
        removeRequest(removed_request_username_);
    } else if (result == REMOVE_REQUEST_CHANNEL_FAILED_ANSWER) {
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Что-то пошло не так при попытке удалить запрос.");
        dialog_ptr->exec();
    }

    if (result == REMOVE_REQUEST_CHAT_SUCCESS_ANSWER) {
        removeRequest(removed_request_username_);
    } else if (result == REMOVE_REQUEST_CHAT_FAILED_ANSWER) {
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Что-то пошло не так при попытке удалить запрос.");
        dialog_ptr->exec();
    }
}

void RequestListDialog::slotAddParticipantResult(int result) {
    std::string text;
    if (result == ADD_PARTICIPANT_TO_PRIVATE_CHANNEL_SUCCESS_ANSWER) {
        text = "Пользователь был добавлен в канал.";
        removeRequest(removed_request_username_);
    } else if (result == ADD_PARTICIPANT_TO_PRIVATE_CHANNEL_FAILED_ANSWER) {
        text = "Что-то пошло не так при попытке добавить пользователя в канал.";
    } else if (result == ADD_PARTICIPANT_TO_CHAT_SUCCESS_ANSWER && !is_channel_) {
        text = "Пользователь был добавлен в чат.";
        removeRequest(removed_request_username_);
    } else if (result == ADD_PARTICIPANT_TO_CHAT_FAILED_ANSWER && !is_channel_){
        text = "Что-то пошло не так при попытке добавить пользователя в чат.";
    }

    std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, text);
    dialog_ptr->exec();
}

void RequestListDialog::removeRequest(const std::string &username) {
    QLayout *layout = scroll_area_->widget()->layout();

    QLayoutItem *widget_item = nullptr;
    for (int i = 0; i < layout->count(); ++i) {
        if (((widget_item = layout->itemAt(i)) != nullptr) && widget_item->widget()) {
            if (static_cast<RequestCard*>(widget_item->widget())->getUsername() == username) {
                layout->removeItem(widget_item);
                delete widget_item->widget();
                delete widget_item;
                break;
            }
        }
    }
}

void RequestListDialog::setupConnections() {
    connect(ClientSingleton::get_client(), &Client::add_private_channel_participant_result, this, &RequestListDialog::slotAddParticipantResult);
    connect(ClientSingleton::get_client(), &Client::remove_request_channel_result,          this, &RequestListDialog::slotRemoveRequestResult);
    connect(ClientSingleton::get_client(), &Client::add_private_chat_participant_result,    this, &RequestListDialog::slotAddParticipantResult);
    connect(ClientSingleton::get_client(), &Client::remove_request_chat_result,             this, &RequestListDialog::slotRemoveRequestResult);
}
