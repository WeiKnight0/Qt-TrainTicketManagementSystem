#include "userreturn.h"
#include "ui_userreturn.h"
#include <QMessageBox>

userReturn::userReturn(DataSystem*input,const QString&str,QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::userReturn),
    data(input),
    psgid(str),
    fromCity(0),
    toCity(0)
{
    ui->setupUi(this);
    //选择模式
    ui->tickettableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);//选择行
    ui->tickettableWidget->setSelectionMode(QAbstractItemView::SingleSelection);//只能选择一行
    ui->tickettableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//不可编辑
    ui->tickettableWidget->setColumnCount(5);
    QStringList ticketlist;
    ticketlist<<"班次号"<<"车厢号"<<"座位号"<<"出发城市"<<"终到城市";
    ui->tickettableWidget->setHorizontalHeaderLabels(ticketlist);
    //初始化两个列表
    deleteptr();
    //开辟列表
    fromCity=new QList<int>;
    toCity=new QList<int>;
    //获得乘客姓名
    QSqlQuery* query=data->getNewQuery();
    if(!query->exec(QString("SELECT Name FROM passengers WHERE ID='%1'").arg(psgid)))
    {
        qDebug()<<query->lastError().text();
        QMessageBox::critical(this,"错误！",QString("系统故障！").append(query->lastError().text()));
        delete query;
        return;
    }
    QString name;
    if(query->next()){
        name=query->value(0).toString();
    }
    ui->label_tt->setText(QString("乘客 %1 (证件号：%2) 的已购班次：").arg(name).arg(psgid));
    //显示购买的所有车次
    //清空表格
    ui->tickettableWidget->clearContents();
    ui->tickettableWidget->setRowCount(0);
    //获得列车列表
    QStringList trainlist=data->getAllTrains();
    for(QString train:trainlist){
        query=data->getNewQuery();
        if(!query->exec(QString("SELECT * FROM %1 WHERE Psgid='%2' ").arg(train.toLower()).arg(psgid))){
            qDebug()<<"错误！";
            delete query;
            QMessageBox::critical(this,"错误！","系统故障！unable to select train");
            delete this;
            return;
        }
        while(query->next()){
            ui->tickettableWidget->insertRow(ui->tickettableWidget->rowCount());
            QTableWidgetItem *item0=new QTableWidgetItem;
            item0->setData(Qt::DisplayRole,train);
            ui->tickettableWidget->setItem(ui->tickettableWidget->rowCount()-1,0,item0);
            for(int i=1;i<=2;i++){
                QTableWidgetItem *item=new QTableWidgetItem;
                item->setData(Qt::DisplayRole,query->value(i));
                ui->tickettableWidget->setItem(ui->tickettableWidget->rowCount()-1,i,item);
            }
            //处理城市，将标号储存在list中
            for(int j=3;j<=4;++j){
                QTableWidgetItem *item3=new QTableWidgetItem;
                if(j==3) fromCity->append(query->value(j).toInt());
                if(j==4) toCity->append(query->value(j).toInt());
                item3->setData(Qt::DisplayRole,data->getCityName(train,query->value(j).toInt()));
                ui->tickettableWidget->setItem(ui->tickettableWidget->rowCount()-1,j,item3);
            }
        }
    }
}

userReturn::~userReturn()
{
    delete ui;
}

void userReturn::on_returnBtn_clicked()
{
    if(QMessageBox::Yes!=QMessageBox::question(this,"提示","您确定要退票吗？")){
        return;
    }
    //获取要退票的信息
    int row=ui->tickettableWidget->currentRow();
    if(row==-1){
        qDebug()<<"没有选择";
        QMessageBox::information(this,"提示","请选择一张已购车票");
        return;
    }
    QString trainclass=ui->tickettableWidget->item(row,0)->text();
    int car=ui->tickettableWidget->item(row,1)->text().toInt();
    int seat=ui->tickettableWidget->item(row,2)->text().toInt();
    int fromIndex=fromCity->at(row);
    int toIndex=toCity->at(row);
    //修改表
    QSqlQuery* query=data->getNewQuery();
    QString exe=QString("DELETE FROM %1 WHERE Psgid='%2' AND Cars=%3 AND Seats=%4 AND FromCity=%5"
                        " AND ToCity=%6").arg(trainclass.toLower()).arg(this->psgid).arg(car).arg(seat).arg(fromIndex).arg(toIndex);
    qDebug()<<exe;
    if(!query->exec(exe)){
        qDebug("ERROR");
        delete query;
        QMessageBox::critical(this,"错误！",QString("系统故障！unable to delete ").append(trainclass));
        delete this;
        return;
    }
    qDebug("退票成功！");
    QMessageBox::information(this,"提示","退票成功！");
    ui->tickettableWidget->removeRow(row);
    ui->tickettableWidget->setRowCount(ui->tickettableWidget->rowCount());
    delete query;
}

void userReturn::on_cancelBtn_clicked()
{
    this->close();
}
