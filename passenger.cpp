#include "passenger.h"

Passenger::Passenger(const QString& name_,bool gender_,const QDate& dt_,const QString& id_,const QString& nation_,const QString& phone_,QObject *parent) :
    QObject(parent),
    name(name_),
    gender(gender_),
    birth(dt_),
    id(id_),
    nation(nation_),
    phone(phone_)
{

}
