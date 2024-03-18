#ifndef TRANSPARENTLABEL_H
#define TRANSPARENTLABEL_H

#include <QLabel>
#include <QGraphicsOpacityEffect>

/**
 * @brief The TransparentLabel class
 * Виджет-наследник класса QLabel, который может менять свою прозрачность.
 */
class TransparentLabel : public QLabel
{
    Q_OBJECT
    Q_PROPERTY (double p_opacity_ READ Opacity WRITE setOpacity NOTIFY opacityChanged)
public:
    TransparentLabel(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());
    TransparentLabel(const QString &text, QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

    void setOpacity(double value);
    double Opacity() const;

private:
    double p_opacity_ = 0;
    QGraphicsOpacityEffect* goe_ = nullptr;

    void setup();

signals:
    void opacityChanged(double value);
};

#endif // TRANSPARENTLABEL_H
