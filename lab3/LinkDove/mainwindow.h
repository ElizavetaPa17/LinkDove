#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void slotSwitchToPage(int index_page);

private:
    Ui::MainWindow *ui;

    /**
     * <p> Настраивает соединения с главным виджетом. </p>
     * @brief setupConnection
     */
    void setupConnection();

};
#endif // MAINWINDOW_H
