#include "returntickets.h"
#include "ui_returntickets.h"
#include<QMessageBox>

ReturnTickets::ReturnTickets(DataSystem* data_input,QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ReturnTickets),
    data(data_input),
    fromCity(0),
    toCity(0)
{
    ui->setupUi(this);
    //设置stackView初始index
    ui->stackedWidget->setCurrentIndex(0);
    //设置表头
    QStringList psglist(QString("姓名#性别#生日#证件号#国家/地区#电话号码").split("#"));
    ui->psgtableWidget->setColumnCount(6);
    ui->psgtableWidget->setHorizontalHeaderLabels(psglist);
    //设置选择模式
    ui->psgtableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);//选择行
    ui->psgtableWidget->setSelectionMode(QAbstractItemView::SingleSelection);//只能选择一行
    ui->psgtableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//不可编辑
    ui->tickettableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);//选择行
    ui->tickettableWidget->setSelectionMode(QAbstractItemView::SingleSelection);//只能选择一行
    ui->tickettableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//不可编辑
    //设置证件号只能输入字母和数字
    ui->idEdit->setAttribute(Qt::WA_InputMethodEnabled, false);
    //设置电话号码只能输入数字
    QValidator *validator=new QIntValidator(ui->phoneEdit);
    ui->phoneEdit->setAttribute(Qt::WA_InputMethodEnabled, false);
    ui->phoneEdit->setValidator(validator);
    //获取所有乘客
    QSqlQuery *query=data->getNewQuery();
    bool re=query->exec("SELECT * FROM passengers");
    if(!re)
    {
        qDebug()<<"ERROR";
        delete query;
        QMessageBox::critical(this,"错误！",QString("系统故障！unable to select passengers "));
        delete this;
        return;
    }
    while(query->next()){
        //qDebug()<<"YYY";
        ui->psgtableWidget->insertRow(ui->psgtableWidget->rowCount());
        for(int i=0;i<6;i++){
            QTableWidgetItem *item=new QTableWidgetItem;
            //qDebug()<<"SSSS"<<query->value(i);
            item->setData(Qt::DisplayRole,query->value(i));
            ui->psgtableWidget->setItem(ui->psgtableWidget->rowCount()-1,i,item);
        }
    }
    delete query;
    //设置ticketTabelWidget
    ui->tickettableWidget->setColumnCount(5);
    QStringList ticketlist;
    ticketlist<<"班次号"<<"车厢号"<<"座位号"<<"出发城市"<<"终到城市";
    ui->tickettableWidget->setHorizontalHeaderLabels(ticketlist);
}

ReturnTickets::~ReturnTickets()
{
    delete ui;
}

void ReturnTickets::on_searchBtn_3_clicked()
{
    qDebug()<<"Start to Search";
    //创立Sql指令
    QString exe(QString("SELECT * FROM passengers "));
    if(ui->NameEdit->text().trimmed().isEmpty()||
            ui->idEdit->text().trimmed().isEmpty()||
            ui->phoneEdit->text().trimmed().isEmpty()){
        QMessageBox::information(this,"提示","姓名/证件号/电话号码均不得为空！");
        return;
    }
    ui->psgtableWidget->clearContents();
    ui->psgtableWidget->setRowCount(0);
    exe.append(QString("WHERE Name='%1' AND ID='%2' AND Phone='%3'")
               .arg(ui->NameEdit->text().trimmed()).arg(ui->idEdit->text().trimmed())
               .arg(ui->phoneEdit->text().trimmed()));

    QSqlQuery *query=data->getNewQuery();
    if(!query->exec(exe)){
        delete query;
        QMessageBox::critical(this,"错误！",QString("系统故障！unable to select passengers "));
        delete this;
        return;
    }
    qDebug()<<exe;
    int SearchCount=0;
    while(query->next()){
        ++SearchCount;
        ui->psgtableWidget->insertRow(ui->psgtableWidget->rowCount());
        for(int i=0;i<6;i++){
            QTableWidgetItem *item=new QTableWidgetItem;
            item->setData(Qt::DisplayRole,query->value(i));
            ui->psgtableWidget->setItem(ui->psgtableWidget->rowCount()-1,i,item);
        }
    }
    delete query;
    if(SearchCount){
        QMessageBox::information(this,"提示",QString("共找到 %1 个乘客信息！").arg(SearchCount));
    }
    else{
        QMessageBox::information(this,"提示","没有找到任何数据！");
    }

}

//查询已购列车
void ReturnTickets::on_showBtn_clicked()
{
    //获取选择的乘客
    int row=ui->psgtableWidget->currentRow();
    if(row==-1){
        qDebug()<<"没有选择";
        QMessageBox::information(this,"提示","请至少选择一名乘客");
        return;
    }
    //初始化两个列表
    deleteptr();
    //开辟列表
    fromCity=new QList<int>;
    toCity=new QList<int>;
    QString name=ui->psgtableWidget->item(row,0)->text();
    QString id=ui->psgtableWidget->item(row,3)->text();
    this->psgid=id;
    ui->label_tt->setText(QString("乘客 %1 (证件号：%2) 的已购班次：").arg(name).arg(id));
    //显示购买的所有车次
    //清空表格
    ui->tickettableWidget->clearContents();
    ui->tickettableWidget->setRowCount(0);
    //获得列车列表
    QStringList trainlist=data->getAllTrains();
    qDebug()<<trainlist;
    for(QString train:trainlist){
        QSqlQuery* query=data->getNewQuery();
        if(!query->exec(QString("SELECT * FROM %1 WHERE Psgid='%2' ").arg(train.toLower()).arg(id))){
            qDebug()<<"错误！";
            delete query;
            QMessageBox::critical(this,"错误！",QString("系统故障！unable to select passengers "));
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
            qDebug()<<*fromCity;qDebug()<<*toCity;
        }
    }
    ui->stackedWidget->setCurrentIndex(1);
}

void ReturnTickets::on_cancelBtn_3_clicked()
{
    this->close();
}

void ReturnTickets::on_toPrevBtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void ReturnTickets::on_cancelBtn_4_clicked()
{
    emit ui->cancelBtn_3->clicked();
}

void ReturnTickets::on_returnBtn_clicked()
{
    if(QMessageBox::Yes!=QMessageBox::question(this,"提示","您确定要退票吗？")){
        return;
    }
    //获取要退票的信息
    int row=ui->tickettableWidget->currentRow();
    if(row==-1){
        qDebug()<<"没有选择";
        QMessageBox::information(this,"提示","请至少选择一名乘客");
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
        QMessageBox::critical(this,"错误！",QString("系统故障！unable to delete passengers"));
        delete this;
        return;
    }
    qDebug("退票成功！");
    QMessageBox::information(this,"提示","退票成功！");
    ui->tickettableWidget->removeRow(row);
    ui->tickettableWidget->setRowCount(ui->tickettableWidget->rowCount());
    delete query;
}
