#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>
#include <QMouseEvent>

/**
 * @brief The ClickableLabel class
 * Класс-нааследник QLabel, генерирующий события при нажатии на него.
 */

class ClickableLabel : public QLabel
{
    Q_OBJECT
public:
    ClickableLabel(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());
    ClickableLabel(const QString &text, QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

signals:
    void clicked();

protected:
    // Перегрузка функции с целью генерации сигнала clicked.
    void mousePressEvent(QMouseEvent *event) override;
};

#endif // CLICKABLELABEL_H
