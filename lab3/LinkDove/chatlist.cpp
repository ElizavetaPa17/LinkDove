#include "chatlist.h"
#include "ui_chatlist.h"

#include <QStyleOption>
#include <QPainter>

#include "typestringdialog.h"
#include "clientsingleton.h"
#include "infodialog.h"
#include "chatcard.h"
#include "utility.h"

ChatList::ChatList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatList)
{
    ui->setupUi(this);

    ui->scrollArea->setWidget(new QWidget());
    ui->scrollArea->widget()->setLayout(new QVBoxLayout());
    ui->scrollArea->widget()->layout()->setAlignment(Qt::AlignTop);

    setupConnection();
}

ChatList::~ChatList()
{
    delete ui;
}

void ChatList::addChat(const ChatInfo &chat_info) {
    ChatCard *chat_card = new ChatCard(this, chat_info);
    connect(chat_card, &ChatCard::chatCardClicked, this, &ChatList::slotHandleChatCardClicked);

    ui->scrollArea->widget()->layout()->addWidget(chat_card);
}

void ChatList::removeChats() {
    ui->searchEdit->setText("");
    QtUtility::clean_layout(ui->scrollArea->widget()->layout());
}

void ChatList::slotClear() {
    removeChats();
}

void ChatList::slotsHandleReturnPress() {
    if (ui->searchEdit->text().isEmpty()) {
        ClientSingleton::get_client()->async_get_chats();
    } else {
        ClientSingleton::get_client()->async_find_chat(ui->searchEdit->text().toStdString());
    }
}

void ChatList::slotFindChatResult(int result) {
    removeChats();
    if (result == FIND_CHAT_FAILED_ANSWER) {
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Чат не найден.");
        dialog_ptr->exec();
    } else {
        ChatInfo chat_info = ClientSingleton::get_client()->get_found_chat();
        addChat(chat_info);
    }
}

void ChatList::slotGetChatsResult(int result, std::vector<ChatInfo> chats) {
    if (result == GET_CHATS_SUCCESS_ANSWER) {
        removeChats();

        for (int i = 0; i < chats.size(); ++i) {
            addChat(chats[i]);
        }
    } else {
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Ошибка получения каналов.");
        dialog_ptr->exec();
    }
}

void ChatList::slotCreateChat() {
    std::unique_ptr<TypeStringDialog> dialog_ptr = std::make_unique<TypeStringDialog>(nullptr, "Введите название чата: ");
    if (dialog_ptr->exec() == QDialog::Accepted) {
        ClientSingleton::get_client()->async_create_chat(dialog_ptr->getString());
    }
}

void ChatList::slotCreateChatResult(int result) {
    std::string text;
    if (result == CREATE_CHAT_SUCCESS_ANSWER) {
        text = "Чат успешно создан.";
        ClientSingleton::get_client()->async_get_chats();
    } else {
        text = "Ошибка создания чата. Попытайтесь позже.";
    }

    std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, text);
    dialog_ptr->exec();
}

void ChatList::slotHandleChatCardClicked(const ChatInfo &chat_info) {
    chat_info_ = chat_info;

    ClientSingleton::get_client()->async_is_banned_chat_user(chat_info.id_);
}

void ChatList::slotHandleIsBannedUser(int result, bool is_banned) {
    std::cerr << "is banned: " << is_banned << '\n';
    if (result == IS_CHAT_BANNED_USER_SUCCESS_ANSWER) {
        if (!is_banned) {
            emit chatCardClicked(chat_info_);
        } else {
            std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Вы были заблокированы в группе. ");
            dialog_ptr->exec();
        }
    } else if (result == IS_CHAT_BANNED_USER_FAILED_ANSWER) {
        std::unique_ptr<InfoDialog> dialog_ptr = std::make_unique<InfoDialog>(nullptr, "Что-то пошло не так при попытке получить статус блокировки в группе. ");
        dialog_ptr->exec();
    }
}

void ChatList::paintEvent(QPaintEvent *) {
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ChatList::setupConnection() {
    connect(ui->searchEdit,                &QLineEdit::returnPressed,   this, &ChatList::slotsHandleReturnPress);
    connect(ui->createChatButton,          &QPushButton::clicked,       this, &ChatList::slotCreateChat);

    connect(ClientSingleton::get_client(), &Client::create_chat_result,    this, &ChatList::slotCreateChatResult);
    connect(ClientSingleton::get_client(), &Client::find_chat_result,      this, &ChatList::slotFindChatResult);
    connect(ClientSingleton::get_client(), &Client::get_chats_result,      this, &ChatList::slotGetChatsResult);
    connect(ClientSingleton::get_client(), &Client::is_banned_user_result, this, &ChatList::slotHandleIsBannedUser);
}
