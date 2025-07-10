#include "train.h"
#include<QSqlRecord>
Train::Train(QString class_in,QDateTime dt_in,QString start,QString end,int cars,int seats,QObject *parent) :
    QObject(parent),
    //数据初始化
    trainclass(class_in),
    StartDT(dt_in),
    StartCity(start),
    EndCity(end),
    CarNum(cars),
    SeatNum(seats)
{}


Train::Train(QObject *parent):QObject(parent){}

