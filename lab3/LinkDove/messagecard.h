#ifndef MESSAGECARD_H
#define MESSAGECARD_H

#include <QWidget>

#include "audiomanager.h"

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
    explicit MessageCard(QWidget *parent, int type, const QString &text);
    explicit MessageCard(QWidget *parent, const QPixmap &pixmap, const QString &pixmap_path);
    explicit MessageCard(QWidget *parent, int type, const QString &text,   const QString &owner_name);
    explicit MessageCard(QWidget *parent, const QPixmap &pixmap, const QString &pixmap_path, const QString &owner_name);

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

    int type_;
    QString string_;

    /**
     * <p> Настраивает соединения в виджете. </p>
     * @brief setupConnection
     */
    void setupConnection();
};

#endif // MESSAGECARD_H
