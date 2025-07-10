#include "viewallpsg.h"
#include "ui_viewallpsg.h"

ViewAllPsg::ViewAllPsg(DataSystem* input,const QString&train,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ViewAllPsg),
    data(input)
{
    ui->setupUi(this);
    //设置标题
    ui->label_tt->setText(QString("列车 %1 的全部乘客信息：").arg(train.toUpper()));
    //设置标题栏
    setWindowTitle(QString("列车 %1 的全部乘客信息").arg(train.toUpper()));
    setWindowFlags(Qt::WindowCloseButtonHint);
    //设置tablewidget
    QStringList list;
    list<<"乘客证件号"<<"车厢号"<<"座位号"<<"出发城市"<<"终到城市";
    ui->tableWidget->setColumnCount(5);
    ui->tableWidget->setHorizontalHeaderLabels(list);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//不可修改
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);//选择行
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);//只能选择一行
    //显示数据
    QSqlQuery* query=data->getNewQuery();
    QString exe=QString("SELECT * FROM %1").arg(train);
    if(!query->exec(exe)){
        delete query;
        delete this;
        return;
    }
    while(query->next()){
        ui->tableWidget->insertRow(ui->tableWidget->rowCount());
        for(int i=0;i<3;++i){
            QTableWidgetItem* item=new QTableWidgetItem;
            item->setData(Qt::DisplayRole,query->value(i));
            ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,i,item);
        }
        for(int j=3;j<5;++j){
            QTableWidgetItem* item=new QTableWidgetItem;
            QString city=data->getCityName(train,query->value(j).toInt());
            item->setData(Qt::DisplayRole,QVariant(city));
            ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,j,item);
        }
    }
    delete query;
}

ViewAllPsg::~ViewAllPsg()
{
    delete ui;
}

void ViewAllPsg::on_returnBtn_clicked()
{
    this->close();
}
