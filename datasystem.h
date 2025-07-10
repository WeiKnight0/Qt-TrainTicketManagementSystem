#ifndef DATASYSTEM_H
#define DATASYSTEM_H

//宏定义
#define TrainParaNum (6)
#define TrainClassIndex (0)
#define StartDateTimeIndex (1)
#define StartCityIndex (2)
#define EndCityIndex (3)
#define CarNumIndex (4)
#define SeatNumIndex (5)
#define PsgParaNum (6)
#define idIndex (3)

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QSqlTableModel>
#include <QTableView>
#include <QDebug>

class DataSystem:public QObject
{
    Q_OBJECT
public:
    explicit DataSystem(QObject* parent=0);
    explicit DataSystem(const QString& path,bool admin,QObject *parent=0);//用于拷贝的临时
    ~DataSystem();
    //连接
    void setConnection(const QString&ip,const QString&name,const QString&acc,const QString&psw);//odbc/mysql
    void setConnection(const QString& path,bool create=false);//sqlite
    void setConnection(const QStringList&);
    //关闭连接
    void closeConnection();
    //检测连接
    bool CheckConnection();
    //获取连接的名称
    QString getDriverName(){
        return db.driverName();
    }
    //创立新的Query
    QSqlQuery* getNewQuery(){
        return new QSqlQuery(db);
    }
    //获取所有的表
    QStringList getAllTables(){
        return db.tables();
    }
    //获取数据库.db位置
    QStringList getDBInfo();
    void createTables(bool createAdmin=true);//创造相应的表
    int getTransferNum(const QString&);//获取中转站数量
    QStringList getAllTrains();//获取所有列车
    QString getCityName(const QString& train,int index);
    int getCityIndex(const QString& train, const QString& city);
    int getRemainingTickets(const QString&, int,int);//获得剩余的票
    bool haveBoughtTickets(const QString&id,const QString&trainname=QString());//检测乘客是否已经买了票

private:
    QSqlDatabase db;
};

#endif // DATASYSTEM_H
