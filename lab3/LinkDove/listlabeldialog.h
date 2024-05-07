#ifndef LISTLABELDIALOG_H
#define LISTLABELDIALOG_H

#include <QDialog>

namespace Ui {
class ListLabelDialog;
}

/**
 * @brief The ListLabelDialog class
 * Диалоговое окно, отображающее информацию в виде списка.
 */
class ListLabelDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ListLabelDialog(QWidget *parent, const std::vector<std::string> &messages);
    ~ListLabelDialog();

private:
    Ui::ListLabelDialog *ui;

    /**
     * <p> Устанавливает список в виджет с указанным содержимым. </p>
     * @brief setLabels
     * @param messages - Содержимое списка для отображения
     */
    void setLabels(const std::vector<std::string> &messages);
};

#endif // LISTLABELDIALOG_H
