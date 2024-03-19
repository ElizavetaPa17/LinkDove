#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QWidget>

class QLineEdit;

/**
 * @brief The LoginWidget class
 * Виджет отображения формы для авторизации (логин-форма)
 */
class LoginWidget : public QWidget
{
    Q_OBJECT
public:
    LoginWidget(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

    QLineEdit *usernameEdit_  = nullptr;
    QLineEdit *emailEdit_     = nullptr;
    QLineEdit *passwordEdit_  = nullptr;

private:
    /**
     * <p> Настраивает общий вид формы. </p>
     * @brief setupDesign
     */
    void setupDesign();
    /**
     * <p> Настраивает дизайн поля. </p>
     * @brief setupTextEditDesign
     * @param - Указатель на поле QLineEdit
     */
    void setupTextEditDesign(QLineEdit *lineEdit);
};

#endif // LOGINWIDGET_H
