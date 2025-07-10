#ifndef TRAINEDITDIALOG_H
#define TRAINEDITDIALOG_H

#include"train.h"//列车类
#include <QDialog>


namespace Ui {
class TrainEditDialog;
}

class TrainEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TrainEditDialog(Train*& address,QWidget *parent = 0);//address 保存列车类的地址
    explicit TrainEditDialog(QList<QVariant>& list,Train*& address,QWidget *parent = 0);//接受列车类的列表
    ~TrainEditDialog();
    void SetTitle(QString str);//设置大标题


private slots:


    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

private:
    Ui::TrainEditDialog *ui;
    Train* &train;//保存编辑完后的列车类
};

#endif // TRAINEDITDIALOG_H
