#ifndef RETURNTICKETS_H
#define RETURNTICKETS_H

#include <QMainWindow>
#include<datasystem.h>

namespace Ui {
class ReturnTickets;
}

class ReturnTickets : public QMainWindow
{
    Q_OBJECT

public:
    explicit ReturnTickets(DataSystem*, QWidget *parent = 0);
    ~ReturnTickets();

private slots:
    void on_searchBtn_3_clicked();

    void on_showBtn_clicked();

    void on_cancelBtn_3_clicked();

    void on_toPrevBtn_clicked();

    void on_cancelBtn_4_clicked();

    void on_returnBtn_clicked();

private:
    Ui::ReturnTickets *ui;
    DataSystem*data;
    //乘客证件号
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

#endif // RETURNTICKETS_H
