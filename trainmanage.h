#ifndef TRAINMANAGE_H
#define TRAINMANAGE_H

#include"datasystem.h"
#include <QMainWindow>
#include"train.h"
#include"edittransfer.h"

namespace Ui {
class TrainManage;
}

class TrainManage : public QMainWindow
{
    Q_OBJECT

public:
    explicit TrainManage(DataSystem* sys,QWidget *parent = 0);
    ~TrainManage();

private slots:
    void on_AddBtn_clicked();

    void on_DeleteBtn_clicked();

    void on_EditBtn_clicked();

    void on_EditTransBtn_clicked();

private:
    Ui::TrainManage *ui;
    DataSystem *data;
    QSqlTableModel *model;
    EditTransfer* edit;
    //数据有效性的检测
    bool isDataValid(Train* train,int index=-1);
    //设置数据
    void setData(Train* train,int index=-1);
    //创建列车班次表
    void CreateTrainTable(Train* train);
    //修改列车班次表
    bool EditTrainTable(Train* train,const QList<QVariant>& list);
    //检测列车是否为空列车
    bool isTrainEmpty(const QString&);
};

#endif // TRAINMANAGE_H
