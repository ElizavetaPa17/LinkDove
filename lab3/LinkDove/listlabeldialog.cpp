#include "listlabeldialog.h"
#include "ui_listlabeldialog.h"

#include <QLabel>

//#include "utility.h"

ListLabelDialog::ListLabelDialog(QWidget *parent, const std::vector<std::string> &messages) :
    QDialog(parent),
    ui(new Ui::ListLabelDialog)
{
    ui->setupUi(this);

    QVBoxLayout *pvboxLayout = new QVBoxLayout;
    pvboxLayout->setSpacing(5);
    ui->scrollArea->setWidget(new QWidget());
    ui->scrollArea->widget()->setLayout(pvboxLayout);
    ui->scrollArea->setWidgetResizable(true);

    setLabels(messages);
}

ListLabelDialog::~ListLabelDialog()
{
    delete ui;
}

void ListLabelDialog::setLabels(const std::vector<std::string> &messages) {
    QHBoxLayout *layout = static_cast<QHBoxLayout*>(ui->scrollArea->widget()->layout());

    QLabel *label = nullptr;
    for (const auto &message : messages) {
        label = new QLabel();
        label->setText(message.c_str());

        layout->addWidget(label);
    }

    layout->addStretch();
}
