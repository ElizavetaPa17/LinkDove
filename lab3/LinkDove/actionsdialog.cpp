#include "actionsdialog.h"
#include "ui_actionsdialog.h"

#include <QStringList>
#include "constants.h"

ActionsDialog::ActionsDialog(QWidget *parent, std::vector<Action> &actions) :
    QDialog(parent),
    ui(new Ui::ActionsDialog)
{
    ui->setupUi(this);
    ui->tableWidget->setColumnCount(2);
    ui->tableWidget->setColumnWidth(0, 200);
    ui->tableWidget->setColumnWidth(1, 200);

    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << tr("Время") << tr("Действие"));

    for (auto& action : actions) {
        ui->tableWidget->insertRow(ui->tableWidget->rowCount());
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,
                                 0,
                                 new QTableWidgetItem(action.time_.toString(DATETIME_FORMAT)));
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,
                                 1,
                                 new QTableWidgetItem(action.description_.c_str()));
    }
}

ActionsDialog::~ActionsDialog()
{
    delete ui;
}
