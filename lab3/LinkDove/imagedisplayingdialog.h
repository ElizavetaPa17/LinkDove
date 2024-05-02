#ifndef IMAGEDISPLAYINGDIALOG_H
#define IMAGEDISPLAYINGDIALOG_H

#include <QDialog>
#include <QLabel>

namespace Ui {
class ImageDisplayingDialog;
}

/**
 * @brief The ImageDisplayingDialog class
 * Окно, отображающее изображение.
 */
class ImageDisplayingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImageDisplayingDialog(QWidget *parent, QPixmap pixmap);
    ~ImageDisplayingDialog();

private:
    Ui::ImageDisplayingDialog *ui;
};

#endif // IMAGEDISPLAYINGDIALOG_H
