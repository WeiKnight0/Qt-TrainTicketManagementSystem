#ifndef TRAIN_H
#define TRAIN_H


#include <QObject>
#include<QString>
#include<QDateTime>
#include<QSqlTableModel>

class Train : public QObject
{
    Q_OBJECT
    friend class TrainManage;
public:
    explicit Train(QString,QDateTime,QString,QString,int,int,QObject *parent = 0);
    explicit Train(QObject *parent = 0);//空的列车类

private:
    //数据
    QString trainclass;
    QDateTime StartDT;
    QString StartCity;
    QString EndCity;
    int CarNum;
    int SeatNum;
};

#endif // TRAIN_H
