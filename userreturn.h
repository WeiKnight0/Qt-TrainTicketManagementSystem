#ifndef USERRETURN_H
#define USERRETURN_H

#include <QMainWindow>
#include"datasystem.h"

namespace Ui {
class userReturn;
}

class userReturn : public QMainWindow
{
    Q_OBJECT

public:
    explicit userReturn(DataSystem*,const QString&,QWidget *parent = 0);
    ~userReturn();

private slots:
    void on_returnBtn_clicked();

    void on_cancelBtn_clicked();

private:
    Ui::userReturn *ui;
    DataSystem* data;
    //乘客证件号
    QString name;
    QString psgid;
    //储存城市标号序列
    QList<int>* fromCity;
    QList<int>* toCity;
    void deleteptr(){
        //删除指针
        if(!fromCity){
            delete fromCity;
            fromCity=0;
        }
        if(!toCity){
            delete toCity;
            toCity=0;
        }
    }
};

#endif // USERRETURN_H
