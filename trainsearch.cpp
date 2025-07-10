#include "trainsearch.h"
#include "ui_trainsearch.h"
#include<QMessageBox>
#include<QDebug>

TrainSearch::TrainSearch(DataSystem* input,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TrainSearch),
    data(input)
{
    ui->setupUi(this);
    setWindowFlags(Qt::WindowCloseButtonHint);
    //设置表格
    QStringList list(QString("列车班次#发车时间#发车城市#终点城市#车厢数#每厢座位数").split("#"));
    ui->tableWidget->setColumnCount(6);
    ui->tableWidget->setHorizontalHeaderLabels(list);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//不可修改
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);//选择行
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);//只能选择一行
    //日期时间框显示当前日期
    ui->dtEdit->setDateTime(QDateTime::currentDateTime());
    //设置搜索项的选择
    connect(ui->TrainCBox,&QCheckBox::stateChanged,[=](int state){
        if(state==2)
            ui->TrainEdit->setEnabled(true);
        else if(state==0)
            ui->TrainEdit->setEnabled(false);
    });
    connect(ui->dtCBox,&QCheckBox::stateChanged,[=](int state){
        if(state==2)
            ui->dtEdit->setEnabled(true);
        else if(state==0)
            ui->dtEdit->setEnabled(false);
    });
    connect(ui->StartCBox,&QCheckBox::stateChanged,[=](int state){
        if(state==2)
            ui->StartEdit->setEnabled(true);
        else if(state==0)
            ui->StartEdit->setEnabled(false);
    });
    connect(ui->EndCBox,&QCheckBox::stateChanged,[=](int state){
        if(state==2)
            ui->EndEdit->setEnabled(true);
        else if(state==0)
            ui->EndEdit->setEnabled(false);
    });
    ui->TrainEdit->setAttribute(Qt::WA_InputMethodEnabled, false);

    //双击以查看乘客
    connect(ui->tableWidget, &QTableWidget::cellDoubleClicked, this,[=](int row){
        //获取列车名
        QString trainclass=ui->tableWidget->item(row,0)->text();
        ViewAllPsg* view=new ViewAllPsg(data,trainclass);
        view->setAttribute(Qt::WA_DeleteOnClose);
        view->exec();
    });

}

TrainSearch::~TrainSearch()
{
    delete ui;
}

void TrainSearch::on_SearchBtn_clicked()//查询
{
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);
    //检测是否四个都取消选择了
    if(!ui->TrainEdit->isEnabled()&&!ui->EndEdit->isEnabled()&&!ui->StartEdit->isEnabled()&&!ui->dtEdit->isEnabled()){
        QMessageBox::information(this,"提示","请至少选择一个查询项目");
        return;
    }
    qDebug()<<"Start to Search";
    //生成指令字符串
    bool empty;//用于检测是否全为空
    bool isFirst=false;//检测是否存在第一个
    //创立Sql指令
    QString exe(QString("select * from trainclass "));
    if(ui->TrainEdit->isEnabled()&&!ui->TrainEdit->text().trimmed().isEmpty()){
        empty=false;
        isFirst=true;
        exe.append(QString("where TrainClass='%1' ").arg(ui->TrainEdit->text().trimmed().toUpper()));
    }
    if(ui->dtEdit->isEnabled()){
        empty=false;
        if(isFirst){
            exe.append("and ");
        }
        else{
            exe.append("where ");
        }
        isFirst=true;
        exe.append(QString("StartDateTime='%1' ").arg(ui->dtEdit->dateTime().toString("yyyy-MM-dd HH:mm")));
    }
    if(ui->StartEdit->isEnabled()&&!ui->StartEdit->text().trimmed().isEmpty()){
        empty=false;
        if(isFirst){
            exe.append("and ");
        }
        else{
            exe.append("where ");
        }
        isFirst=true;
        exe.append(QString("StartCity='%1' ").arg(ui->StartEdit->text().trimmed()));
    }
    if(ui->EndEdit->isEnabled()&&!ui->EndEdit->text().trimmed().isEmpty()){
        empty=false;
        if(isFirst){
            exe.append("and ");
        }
        else{
            exe.append("where ");
        }
        isFirst=true;
        exe.append(QString("EndCity='%1' ").arg(ui->EndEdit->text().trimmed()));
    }
    qDebug()<<exe;

    if(empty){
        QMessageBox::information(this,"提示","请至少选择一个查询项");
        return;
    }
    QSqlQuery *query=this->data->getNewQuery();
    query->exec(exe);
    //添加数据
    int SearchCount=0;//统计数量
    while(query->next()){
        ++SearchCount;
        ui->tableWidget->insertRow(ui->tableWidget->rowCount());
        for(int i=0;i<6;i++){
            QTableWidgetItem *item=new QTableWidgetItem;
            item->setData(Qt::DisplayRole,query->value(i));
            ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,i,item);
        }
    }
    delete query;
    if(SearchCount){
        QMessageBox::information(this,"提示",QString("共找到 %1 个班次信息！").arg(SearchCount));
    }
    else{
        QMessageBox::information(this,"提示","没有找到任何数据！");
    }
}


void TrainSearch::on_ReturnBtn_clicked()
{
    this->close();
}
