#ifndef COMPLAINTCARD_H
#define COMPLAINTCARD_H

#include <QWidget>

class Complaint;

namespace Ui {
class ComplaintCard;
}

/**
 * @brief The ComplaintCard class
 * Виджет жалобы в списке жалоб.
 */
class ComplaintCard : public QWidget
{
    Q_OBJECT

public:
    explicit ComplaintCard(QWidget *parent, const Complaint& complaint);
    ~ComplaintCard();

    /**
     * <p> Возвращает идентификатор отображаемой жалобы. </p>
     * @brief getComplaintId
     * @return
     */
    unsigned long long getComplaintId();

signals:
    /**
     * <p> Сигнал генерируется при попытке удалить жалобу. /p>
     * @brief removed
     * @param id - Идентификатор удаляемой жалобы.
     */
    void removed(unsigned long long id);

private slots:
    /**
     * <p> Слот, обрабатыващий нажатие кнопки отправления ответа на жалобу пользователю. </p>
     * @brief slotAnswerUser
     */
    void slotAnswerUser();

protected:
    // Переопределяем метод отрисовки, т.к. иначе с виджетом
    // не работают css-таблицы (проблема самого  Qt)
    void paintEvent(QPaintEvent *) override;

private:
    Ui::ComplaintCard *ui;
    unsigned long long complaint_id_;

    /**
     * <p> Настраивает соединения внутри виджета. </P>
     * @brief setupConnection
     */
    void setupConnections();
};

#endif // COMPLAINTCARD_H
