#ifndef COMPLAINTSLISTDIALOG_H
#define COMPLAINTSLISTDIALOG_H

#include <QDialog>

class Complaint;
class QScrollArea;

/**
 * @brief The ComplaintsListDialog class
 * Виджет отображения всех жалоб.
 */
class ComplaintsListDialog : public QDialog
{
    Q_OBJECT
public:
    ComplaintsListDialog(QWidget *parent = nullptr);
    ~ComplaintsListDialog();

    /**
     * <p> Добавляет жалобу в отображаемый список. </p>
     * @brief addComplaint
     * @param complaint - Добавляемая жалоба.
     */
    void addComplaint(const Complaint& complaint);

    /**
     * <p> Удаляет жалобу из отображаемого списка. </p>
     * @brief removeComplaint
     * @param complaint_id - Идентификатор удаляемой жалобы.
     */
    void removeComplaint(unsigned long long complaint_id);

    /**
     * <p> Удаляет все жалобы из отображаемого списка. </p>
     * @brief removeAllComplaints
     */
    void removeAllComplaints();

public slots:
    /**
     * <p> Слот, отображающий результат удаления жалобы. </p>
     * @brief slotDeleteComplaintResult
     * @param result - Результат удаления жалобы.
     */
    void slotDeleteComplaintResult(int result);

private:
    QScrollArea *scroll_area_;
};

#endif // COMPLAINTSLISTDIALOG_H
