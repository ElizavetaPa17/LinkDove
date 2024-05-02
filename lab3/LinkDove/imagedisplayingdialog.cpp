#include "imagedisplayingdialog.h"
#include "ui_imagedisplayingdialog.h"

ImageDisplayingDialog::ImageDisplayingDialog(QWidget *parent, QPixmap pixmap) :
    QDialog(parent),
    ui(new Ui::ImageDisplayingDialog)
{
    ui->setupUi(this);

    ui->imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    ui->imageLabel->setScaledContents(true);

    ui->imageLabel->setPixmap(pixmap);
}

ImageDisplayingDialog::~ImageDisplayingDialog()
{
    delete ui;
}
