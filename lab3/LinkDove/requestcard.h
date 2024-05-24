#ifndef REQUESTCARD_H
#define REQUESTCARD_H

#include <QWidget>

namespace Ui {
class RequestCard;
}

class RequestCard : public QWidget
{
    Q_OBJECT

public:
    explicit RequestCard(QWidget *parent, const std::string &username);
    ~RequestCard();

    /**
     * <p> Возвращает никнейм пользователя. </p>
     * @brief getUsername
     * @return
     */
    std::string getUsername();

signals:
    /**
     * <p> Сигнал генерируется при нажатии на кнопку добавления. . /p>
     * @brief add
     * @param username - Никнейм добавляемого пользователя.
     */
    void add(std::string username);

    /**
     * <p> Сигнал генерируется при попытке удалить запрос. /p>
     * @brief removed
     * @param username - Никнейм добавляемого пользователя.
     */
    void remove(std::string username);

private:
    Ui::RequestCard *ui;
    std::string username_;

    /**
     * <p> Настраивает соединения внутри виджета. </P>
     * @brief setupConnection
     */
    void setupConnections();
};

#endif // REQUESTCARD_H
