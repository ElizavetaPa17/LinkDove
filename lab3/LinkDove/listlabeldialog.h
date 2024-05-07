#ifndef LISTLABELDIALOG_H
#define LISTLABELDIALOG_H

#include <QDialog>

namespace Ui {
class ListLabelDialog;
}

class ListLabelDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ListLabelDialog(QWidget *parent = nullptr);
    ~ListLabelDialog();

    /**
     * <p> Устанавливает список в виджет с указанным содержимым. </p>
     * @brief setLabels
     * @param messages - Содержимое списка для отображения
     */
    void setLabels(std::vector<std::string> messages);

private:
    Ui::ListLabelDialog *ui;
};

#endif // LISTLABELDIALOG_H
