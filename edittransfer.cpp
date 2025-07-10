#include "edittransfer.h"
#include "ui_edittransfer.h"
#include <QInputDialog>
#include <QPushButton>
#include<QMessageBox>

EditTransfer::EditTransfer(DataSystem*data_input,const QString& name,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditTransfer),
    data(data_input),
    trainclass(name)
{
    ui->setupUi(this);
    setWindowFlags(Qt::WindowCloseButtonHint);
    //设置发车和终点
    QSqlQuery*query=data->getNewQuery();
    if(!query->exec(QString("SELECT StartCity,EndCity FROM trainclass WHERE TrainClass='%1'").arg(name))){
        QMessageBox::critical(this,"错误！","系统故障！failed to select cities");
        return;
    }
    while(query->next()){
        ui->StartLabel->setText(query->value(0).toString());
        ui->EndLabel->setText(query->value(1).toString());
    }
    //设置中间站的显示
    delete query;
    query=data->getNewQuery();
    ui->tableWidget->setColumnCount(1);
    QStringList list;
    list.append("城市");
    ui->tableWidget->setHorizontalHeaderLabels(list);
    //qDebug()<<data->getTransferNum(name);
    for(int i =1;i<=data->getTransferNum(name);i++){
        if(!query->exec(QString("SELECT City FROM transfer_%1 WHERE indexNum=%2")
                        .arg(name.toLower()).arg(i))){
            qDebug()<<"unable to select city";
            QMessageBox::critical(this,"错误！","系统故障！failed to select cities");
            return;
        }
        ui->tableWidget->insertRow(ui->tableWidget->rowCount());
        while(query->next()){
            QTableWidgetItem*item=new QTableWidgetItem;
            //qDebug()<<"VALUE"<<query->value(j);
            item->setData(Qt::DisplayRole,query->value(0));
            ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,0,item);
    }

    }
    delete query;

}

EditTransfer::~EditTransfer()
{
    delete ui;
}

void EditTransfer::on_addBtn_clicked()
{
    //创立新的编辑窗口
    bool ok;
    QString text = QInputDialog::getText(nullptr, "编辑中间站", "请输入中间站", QLineEdit::Normal,"" ,
                                         &ok,windowFlags()&~Qt::WindowCloseButtonHint&~Qt::WindowContextHelpButtonHint);
    if(ok&&text.trimmed().isEmpty()){
        QMessageBox::information(this,"提示","站名不能为空！");
        emit ui->addBtn->clicked();
        return;
    }
    if (ok && !text.trimmed().isEmpty()) {
        qDebug()<<"尝试添加"<<text;
        //检测是否重复
        if(data->getCityIndex(this->trainclass,text.trimmed())==-3){
            delete this;
            return;
        }
        if(data->getCityIndex(this->trainclass,text.trimmed())!=-2){
            QMessageBox::information(this,"提示","车站名与现有车站重复！");
            return;
        }
        //添加新行
        ui->tableWidget->insertRow(ui->tableWidget->rowCount());
        //创造新item并添加
        QTableWidgetItem* item= new QTableWidgetItem;
        item->setData(Qt::DisplayRole,QVariant(text));
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,0,item);
    }

}

void EditTransfer::on_deleteBtn_clicked()
{
    int row=ui->tableWidget->currentRow();
    ui->tableWidget->removeRow(row);
}

void EditTransfer::on_cancelBtn_clicked()
{
    this->close();
}

void EditTransfer::on_confirmBtn_clicked()
{
    //获取目前的中转站个数
    int prevtransNum=data->getTransferNum(this->trainclass);
    //获取修改过的中转站个数
    int posttransNum=ui->tableWidget->rowCount();
    qDebug()<<prevtransNum<<posttransNum;

    qDebug()<<"开始删除";
    QSqlQuery* query=data->getNewQuery();
    if(prevtransNum>posttransNum){//删去多余的
        for(int i=posttransNum+1;i<=prevtransNum;++i){
            QString exedel=QString("DELETE FROM transfer_%1 WHERE indexNum=%2").arg(trainclass).arg(i);
            if(!query->exec(exedel)){
                qDebug()<<"删除失败！"<<i;
                delete query;
                return;
            }
        }
    }
    qDebug()<<"开始添加";
    if(prevtransNum<posttransNum){//增加不足的
        for(int i=prevtransNum+1;i<=posttransNum;++i){
            QString exeadd=QString("INSERT INTO transfer_%1(indexNum) VALUES(%2)").arg(trainclass).arg(i);
            if(!query->exec(exeadd)){
                qDebug()<<"增加失败！"<<i;
                delete query;
                return;
            }
        }
    }
    //更新数据
    qDebug()<<"开始更新";
    for(int i=1;i<=posttransNum;i++){
        QString exeupd=QString("UPDATE transfer_%1 SET City='%2' WHERE indexNum=%3")
                .arg(trainclass).arg(ui->tableWidget->item(i-1,0)->text()).arg(i);
        if(!query->exec(exeupd)){
            qDebug()<<"修改失败！"<<i;
            delete query;
            return;
        }
    }
    qDebug()<<"修改成功！";
    QMessageBox::information(this,"提示","修改中间站成功！");
    this->close();
}

void EditTransfer::on_upBtn_clicked()
{

    int row = ui->tableWidget->currentRow();
    if (row < 0 || row -1 < 0 ) {//不能上移
        qDebug()<<"不能上移";
        return; // No row to move
    }
    QTableWidgetItem* upItem=ui->tableWidget->takeItem(row-1,0);
    ui->tableWidget->insertRow(row+1);
    ui->tableWidget->setItem(row+1,0,upItem);
    ui->tableWidget->removeRow(row-1);
    qDebug()<<"上移";
}

void EditTransfer::on_downBtn_clicked()
{
    int row = ui->tableWidget->currentRow();
    if (row < 0 || row +1 >=ui->tableWidget->rowCount() ) {//不能下移
        qDebug()<<"不能下移";
        return; // No row to move
    }
    QTableWidgetItem* downItem=ui->tableWidget->takeItem(row+1,0);
    ui->tableWidget->insertRow(row);
    ui->tableWidget->setItem(row,0,downItem);
    ui->tableWidget->removeRow(row+2);
    qDebug()<<"下移";
}
