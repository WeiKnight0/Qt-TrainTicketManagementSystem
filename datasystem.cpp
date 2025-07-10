#include "datasystem.h"
#include<exception>
#include<QMessageBox>
#include <QDir>
#include<QCoreApplication>
#include<QFile>
DataSystem::DataSystem(QObject* parent)
    :QObject(parent)
{
    //检测文件存在才配置
    QFile traindb(QString(QDir::currentPath()).append(QString("/data/traindb.db")));
    if(traindb.exists()){
        db=QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("data/traindb.db");//文件路径
        if(!CheckConnection()){
            return;
        }
        createTables();
    }
    else{
        qDebug("数据库不存在");
    }

}
DataSystem::DataSystem(const QString& path,bool admin,QObject *parent):
    QObject(parent)
{
    //QSQLITE
    db=QSqlDatabase::addDatabase("QSQLITE","tempconnection");
    db.setDatabaseName(path);
    if(!CheckConnection()){
        return;
    }
    createTables(admin);
}

DataSystem::~DataSystem(){
    closeConnection();
}

void DataSystem::closeConnection(){
    if(db.isOpen()){
        db.close();
        QSqlDatabase::removeDatabase(db.connectionName());
    }
}

void DataSystem::setConnection(const QString &ip, const QString &name, const QString &user, const QString &psw){
    QSqlDatabase::removeDatabase(db.connectionName());
    db.close();
    db=QSqlDatabase::addDatabase("QODBC");
    db.setHostName(ip);
    db.setDatabaseName(name);
    db.setUserName(user);
    db.setPassword(psw);
}

void DataSystem::setConnection(const QString &path,bool create){
    if(!create){
        QFile file(path);
        if(!file.exists()){
            QMessageBox::information(0,"提示","文件不存在！");
            return;
        }
    }
    QSqlDatabase::removeDatabase(db.connectionName());
    db.close();
    db=QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path);
}
void DataSystem::setConnection(const QStringList &list){
    if(list.count()==4){
        setConnection(list.at(0),list.at(1),list.at(2),list.at(3));
        return;
    }
    else if(list.count()==1){
        setConnection(list.at(0));
        return;
    }
    else{
        QMessageBox::critical(0,"错误","无法初始化数据库！");
    }
}

QStringList DataSystem::getDBInfo(){
    if(getDriverName()=="QSQLITE"){
        QDir dir(db.databaseName());
        QString path = dir.absolutePath();
        return QStringList(path);
    }
    else if(getDriverName()=="QODBC"){
        QString ip=db.hostName();
        QString name=db.databaseName();
        QString user=db.userName();
        QString psw=db.password();
        QStringList list;
        list<<ip<<name<<user<<psw;
        return list;
    }
    else{
        return QStringList();//空
    }
}

bool DataSystem::CheckConnection(){
    //测试连接
    bool re=db.open();
    if (!re) {
        qDebug()<<db.lastError().text();
    }
    return re;
}

int DataSystem::getTransferNum(const QString &name){
    QSqlQuery* query=getNewQuery();
    if(!query->exec(QString("SELECT * FROM transfer_%1").arg(name))){
        qDebug()<<"无法搜索";
        return -1;
    }
    int Num=0;
    while(query->next()){
        ++Num;
    }
    delete query;
    return Num-2;
}

QStringList DataSystem::getAllTrains(){
    QSqlQuery *query=getNewQuery();
    query->exec("SELECT TrainClass FROM trainclass");
    QStringList list;
    while(query->next()){
        list.append(query->value(0).toString());
    }
    delete query;
    return list;
}

QString DataSystem::getCityName(const QString & train, int index){
    QSqlQuery* query=getNewQuery();
    query->exec(QString("SELECT City FROM transfer_%1 WHERE indexNum=%2 ").arg(train).arg(index));
    QString city;
    while(query->next()){
        city=query->value(0).toString();
        delete query;
        return city;
    }
    delete query;
    return QString();
}

int DataSystem::getCityIndex(const QString &train, const QString &city){
    QSqlQuery* query=getNewQuery();
    if(!query->exec(QString("SELECT * FROM transfer_%1 WHERE City='%2' ")
                    .arg(train).arg(city)))
    {
        delete query;
        QMessageBox::critical(0,"错误！","系统故障！failed to select end city");
        return -3;//表示系统故障
    }//查找失败
    int index=-2;//-2表示没找到
    if(query->next())
    {
        index=query->value(0).toInt();
    }
    return index;
}

int DataSystem::getRemainingTickets(const QString &train, int start, int end){
    QSqlQuery* query=getNewQuery();
    query->exec(QString("SELECT CarNum,SeatNum FROM trainclass where TrainClass='%1' ").arg(train.toUpper()));
    int carnum=0;
    int seatnum=0;
    while(query->next()){
        qDebug()<<query->value(0)<<query->value(1);
        carnum=query->value(0).toInt();
        seatnum=query->value(1).toInt();
    }
    delete query;
    int num=0;
    for(int i=1;i<=carnum;i++){
        for(int j=1;j<=seatnum;j++){
            query=getNewQuery();
            QString exe=QString("SELECT * FROM %1 WHERE Cars=%2 AND Seats=%3")
                    .arg(train.toLower()).arg(i)
                    .arg(j);
            //qDebug()<<exe;
            if(!query->exec(exe)){
                qDebug()<<"failed to select trains";
                QMessageBox::critical(nullptr,"错误！","系统故障！failed to select trains");
                delete this;
                return -1;
            }
            while(query->next()){
                int start_=query->value(3).toInt();//起始
                int end_=query->value(4).toInt();//终到
                if(!((end_!=-1&&end_<=start)||(start_>=end&&end!=-1))){
                    num++;
                    break;
                }
            }
            delete query;
        }
        }
    return carnum*seatnum-num;
}

bool DataSystem::haveBoughtTickets(const QString &id,const QString&trainname){
    QSqlQuery* query=getNewQuery();
    QStringList trainlist=getAllTrains();
    if(trainname.isEmpty()){
        for(QString train:trainlist){
            query->exec(QString("SELECT * FROM %1 WHERE Psgid='%2'").arg(train.toLower()).arg(id));
            if(query->next()){
                delete query;
                return true;
            }
        }
    }
    else{
        if(trainlist.contains(trainname)){
            if(!query->exec(QString("SELECT * FROM %1 WHERE Psgid='%2'").arg(trainname.toLower()).arg(id))){
                QMessageBox::critical(0,"错误","unable to select psg id in train");
                delete query;
                delete this;
                return 0;
            }
            if(query->next()){
                qDebug()<<id<<"已经在"<<trainname<<"买了票";
                return true;
            }
        }
        else{
            return 0;
        }
    }
    delete query;
    return false;
}

void DataSystem::createTables(bool createAdmin)
{
    QSqlQuery* query=getNewQuery();
    QString traintable=QString("CREATE TABLE IF NOT EXISTS trainclass"
                               "(TrainClass varchar(20) NOT NULL,"
                               "StartDateTime datetime NOT NULL,"
                               "StartCity varchar(255) NOT NULL,"
                               "EndCity varchar(255) NOT NULL,"
                               "CarNum int,"
                               "SeatNum int"
                               ")");
    if(!query->exec(traintable)){
            delete query;
        throw std::runtime_error("unable to create trainclass table");
        return;
    }
    QString psgtable=QString("CREATE TABLE IF NOT EXISTS passengers("
                               "Name varchar(255) NOT NULL,"
                               "Gender bit NOT NULL,"
                               "Birthday date NOT NULL,"
                               "ID varchar(255) NOT NULL,"
                               "Nationality varchar(20) NOT NULL,"
                               "Phone varchar(20) NOT NULL"
                               ")");
    if(!query->exec(psgtable)){
            delete query;
        throw std::runtime_error("unable to create passengers table");
        return;
    }
    QString userstable=QString("CREATE TABLE IF NOT EXISTS users("
                               "account varchar(255) NOT NULL,"
                               "passcode varchar(255) NOT NULL,"
                               "relatedID varchar(255) NOT NULL"
                               ")");
    if(!query->exec(userstable)){
            delete query;
        throw std::runtime_error("unable to create admin table");
    }
    QString adminstable=QString("CREATE TABLE IF NOT EXISTS admin("
                               "account varchar(255) NOT NULL,"
                               "passcode varchar(255) NOT NULL"
                               ")");
    if(!query->exec(adminstable)){
            delete query;
        throw std::runtime_error("unable to create admin table");
                    return;
    }
    if(createAdmin){
        //创建默认管理员账户
        QString findamin=QString("SELECT * FROM admin WHERE account='admin'");
        if(!query->exec(findamin)){
            qDebug()<<query->lastError().text();
            delete query;
            throw std::runtime_error("unable to select admin");
            return;
        }
        if(!query->next()){
            QString setadmin=QString("INSERT INTO admin VALUES('admin','123456')");
            if(!query->exec(setadmin)){
                    delete query;
                throw std::runtime_error("unable to set admin");
                return;
            }
        }
    }
    delete query;
}
