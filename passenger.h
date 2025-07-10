#ifndef PASSENGER_H
#define PASSENGER_H


#include <QObject>
#include<QString>
#include<QDate>

class Passenger : public QObject
{
    Q_OBJECT
    friend class PassengerManage;//友元
public:
    explicit Passenger(const QString&,bool,const QDate&,const QString&,const QString&,const QString&,QObject *parent = 0);


signals:

public slots:

private:
    QString name;
    bool gender;
    QDate birth;
    QString id;
    QString nation;
    QString phone;
};

#endif // PASSENGER_H
