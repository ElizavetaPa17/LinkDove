#ifndef MESSAGECARD_H
#define MESSAGECARD_H

#include <QWidget>

namespace Ui {
class MessageCard;
}

class MessageCard : public QWidget
{
    Q_OBJECT

public:
    explicit MessageCard(QWidget *parent, const QString &text);
    explicit MessageCard(QWidget *parent, const QPixmap &pixmap, const QString &pixmap_path);
    ~MessageCard();

protected:
    explicit MessageCard(QWidget *parent = nullptr);

    // Переопределяем метод отрисовки, т.к. иначе с виджетом
    // не работают css-таблицы (проблема самого  Qt)
    void paintEvent(QPaintEvent *);

    // Переопределяем метод нажатия на сообщения для
    // возможности отображения картинки с отдельном окне
    void mousePressEvent(QMouseEvent *event) override;

private:
    Ui::MessageCard *ui;
    bool image_displaying_;
    QString pixmap_path_;
};

#endif // MESSAGECARD_H
