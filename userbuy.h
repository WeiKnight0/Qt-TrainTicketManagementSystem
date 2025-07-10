#ifndef USERBUY_H
#define USERBUY_H

#include <QMainWindow>
#include"datasystem.h"

namespace Ui {
class userBuy;
}

class userBuy : public QMainWindow
{
    Q_OBJECT

public:
    explicit userBuy(DataSystem*,const QString&,QWidget *parent = 0);
    ~userBuy();

private slots:
    void on_searchBtn_clicked();

    void on_buyBtn_clicked();

    void on_toPrevBtn_clicked();

    void on_cancelBtn_3_clicked();

    void on_cancelBtn_clicked();

    void on_cancelBtn_2_clicked();

    void on_confirmBtn_clicked();

    void on_toPrevBtn_2_clicked();

    void on_checkBox_stateChanged(int arg1);

    void on_confirmBtn_2_clicked();

private:
    Ui::userBuy *ui;
    DataSystem* data;
    QList<int>* startlist;
    QList<int>* endlist;
    void deleteptr(){
        //删除指针
        if(!startlist){
            delete startlist;
            startlist=0;
        }
        if(!endlist){
            delete endlist;
            endlist=0;
        }
    }
    //自己的信息
    QString selfname;
    QString selfid;
    QString selfphone;
    //列车信息
    QString trainclass;
    int carnum;
    int seatnum;
    int startindex;//出发城市的序号
    QString start;
    int endindex;//终到城市的序号
    QString end;
    //乘客信息
    QString name;
    QString id;
    QString phone;
    int car;
    int seat;
};

#endif // USERBUY_H
