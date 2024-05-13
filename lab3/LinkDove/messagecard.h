#ifndef MESSAGECARD_H
#define MESSAGECARD_H

#include <QWidget>

#include "audiomanager.h"
#include "imessage.h"

namespace Ui {
class MessageCard;
}

/**
 * @brief The MessageCard class
 * Виджет, отображающий сообщение.
 */
class MessageCard : public QWidget
{
    Q_OBJECT

public:
    explicit MessageCard(QWidget *parent, std::shared_ptr<IMessage> msg);
    ~MessageCard();

protected:
    explicit MessageCard(QWidget *parent = nullptr);

    // Переопределяем метод отрисовки, т.к. иначе с виджетом
    // не работают css-таблицы (проблема самого  Qt)
    void paintEvent(QPaintEvent *);

    // Переопределяем метод нажатия на сообщения для
    // возможности отображения картинки с отдельном окне
    void mousePressEvent(QMouseEvent *event) override;

private slots:
    /**
     * <p> Слот, который обрабатывает событие нажатия на кнопку воспроизведения звука. </p>
     * @brief slotPlayAudio
     */
    void slotPlayAudio();

    /**
     * <p> Слот, который обрабатывает событие нажатия на кнопку остановки звука. </p>
     * @brief slotPlayAudio
     */
    void slotStopAudio();

private:
    Ui::MessageCard *ui;

    std::shared_ptr<IMessage> msg_;

    /**
     * <p> Настраивает соединения в виджете. </p>
     * @brief setupConnection
     */
    void setupConnection();
};

#endif // MESSAGECARD_H
