#ifndef PSGEDITDIALOG_H
#define PSGEDITDIALOG_H

#include"passenger.h"
#include <QDialog>
#include<QList>
#include<QVariant>

namespace Ui {
class PsgEditDialog;
}

class PsgEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PsgEditDialog(Passenger*&, QWidget *parent = 0);
    explicit PsgEditDialog(Passenger *&,QList<QVariant>&, QWidget *parent=0);
    ~PsgEditDialog();
    void SetTitle(const QString&);
private slots:
    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

private:
    Ui::PsgEditDialog *ui;
    Passenger*& psg;//保存乘客类

    bool getGender();//获取性别
    void setGender(bool);//设置性别
};

#endif // PSGEDITDIALOG_H
