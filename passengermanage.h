#ifndef PASSENGERMANAGE_H
#define PASSENGERMANAGE_H

#include <QMainWindow>
#include"datasystem.h"
#include"passenger.h"

namespace Ui {
class PassengerManage;
}

class PassengerManage : public QMainWindow
{
    Q_OBJECT

public:
    explicit PassengerManage(DataSystem*,QWidget *parent = 0);
    ~PassengerManage();

private slots:
    void on_AddBtn_clicked();

    void on_DeleteBtn_clicked();

    void on_EditBtn_clicked();

private:
    Ui::PassengerManage *ui;
    DataSystem* data;
    QSqlTableModel *model;
    //数据有效性的检测
    bool isDataInvalid(Passenger* psg,int index=-1);
    //设置数据
    void setData(Passenger *psg,int index=-1);//用于修改
};

#endif // PASSENGERMANAGE_H
