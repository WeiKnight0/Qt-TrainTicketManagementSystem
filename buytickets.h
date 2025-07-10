#ifndef BUYTICKETS_H
#define BUYTICKETS_H

#include <QMainWindow>
#include"datasystem.h"

namespace Ui {
class BuyTickets;
}

class BuyTickets : public QMainWindow
{
    Q_OBJECT

public:
    explicit BuyTickets(DataSystem*, QWidget *parent = 0);
    ~BuyTickets();

private slots:
    void on_searchBtn_clicked();

    void on_buyBtn_clicked();

    void on_cancelBtn_clicked();

    void on_toPrevBtn_clicked();

    void on_cancelBtn_2_clicked();

    void on_confirmBtn_clicked();

    void on_checkBox_stateChanged(int arg1);

    void on_searchBtn_2_clicked();

    void on_toPrevBtn_2_clicked();

    void on_cancelBtn_3_clicked();

    void on_confirmBtn_2_clicked();

private:
    Ui::BuyTickets *ui;
    DataSystem *data;
    //临时存储选择的列车数据
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

#endif // BUYTICKETS_H
