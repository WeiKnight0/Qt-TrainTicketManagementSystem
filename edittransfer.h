#ifndef EDITTRANSFER_H
#define EDITTRANSFER_H

#include <QDialog>
#include"datasystem.h"

namespace Ui {
class EditTransfer;
}

class EditTransfer : public QDialog
{
    Q_OBJECT

public:
    explicit EditTransfer(DataSystem*,const QString&, QWidget *parent = 0);
    ~EditTransfer();

private slots:
    void on_addBtn_clicked();

    void on_deleteBtn_clicked();

    void on_cancelBtn_clicked();

    void on_confirmBtn_clicked();

    void on_upBtn_clicked();

    void on_downBtn_clicked();

private:
    Ui::EditTransfer *ui;
    DataSystem*data;
    QString trainclass;
};

#endif // EDITTRANSFER_H
