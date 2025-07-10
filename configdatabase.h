#ifndef CONFIGDATABASE_H
#define CONFIGDATABASE_H

#include <QDialog>
#include"datasystem.h"
#include<QDebug>

namespace Ui {
class ConfigDatabase;
}

class ConfigDatabase : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigDatabase(DataSystem*,QWidget *parent = 0);
    ~ConfigDatabase();

private slots:
    void on_confirmBtn_clicked();

    void on_chooseBtn_clicked();

    void on_cancelBtn_clicked();

private:
    Ui::ConfigDatabase *ui;
    DataSystem* data;
    //数据库信息
    //ODBC/mysql
    QString ip;
    QString dataname;
    QString acc;
    QString psw;
    //sqlite
    QString path;
};

#endif // CONFIGDATABASE_H
