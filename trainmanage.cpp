#include "trainmanage.h"
#include "ui_trainmanage.h"
#include"traineditdialog.h"//列车班次添加修改的通用界面
#include"train.h"//列车类
#include<QDialog>
#include<QStringList>
#include<QStandardItem>
#include<QMessageBox>
#include<QString>
#include<exception>


TrainManage::TrainManage(DataSystem*sys,QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TrainManage),
    data(sys)
{
    ui->setupUi(this);
    model=new QSqlTableModel;
    model->setTable("trainclass");
    //设置表头
    QStringList list(QString("列车班次#发车时间#发车城市#终点城市#车厢数#每厢座位数").split("#"));
    [&](QStringList& mylist)->void{
        for(int i=0;i<TrainParaNum;i++){
            model->setHeaderData(i,Qt::Horizontal,mylist.at(i));
        }
    }(list);
    //设置表格模型
    ui->tableView->setModel(model);
    model->select();
    //设置选择模式
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);//选择行
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);//只能选择一行
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);//不可编辑

}

TrainManage::~TrainManage()
{
    delete ui;
    delete model;
}
//检测数据是否有效
bool TrainManage::isDataValid(Train *train,int index){
    if(!train){
        return 0;
    }
    if(index!=-1){//修改时
        const QString MyTrainClass=model->data(model->index(index,TrainClassIndex)).toString();
        const QString MySC=model->data(model->index(index,StartCityIndex)).toString().trimmed();
        const QString MyED=model->data(model->index(index,EndCityIndex)).toString().trimmed();
        const int MyCarNum=model->data(model->index(index,CarNumIndex)).toInt();
        const int MySeatNum=model->data(model->index(index,SeatNumIndex)).toInt();
        //先检测座位号
        if(train->CarNum<MyCarNum||train->SeatNum<MySeatNum){//调小了车箱数和座位数
            //判断是否为空列车
            if(!isTrainEmpty(MyTrainClass)){
                qDebug("not empty train");
                QMessageBox::information(this,"提示","非空列车不允许调整车厢数和座位数！");
                return false;//表示无效
            }
            else{
                qDebug("empty train");
            }
        }
        if(train->StartCity!=MySC||train->EndCity!=MyED)//列车出发和终点城市被修改时
        {
            if(!isTrainEmpty(MyTrainClass)){
                qDebug("not empty train");
                QMessageBox::information(this,"提示","非空列车不允许调整出发和终点城市！");
                return false;//表示无效
            }
            else{
                qDebug("empty train");
            }
        }
        if(train->trainclass.toLower()==MyTrainClass.toLower())//如果没有修改列车班次号，则一定可以修改
        {
            //检测出发站和终点站是否重复
            const QString SC=train->StartCity;
            const QString ED=train->EndCity;
            if(SC==ED){
                QMessageBox::critical(this,"错误！","出发城市和终点城市重复！");
                return false;
            }
            return true;//表示有效
        }
    }
    //检测出发站和终点站是否重复
    const QString MySC=train->StartCity;
    const QString MyED=train->EndCity;
    qDebug()<<"SC"<<MySC<<"ED"<<MyED<<(MySC==MyED);
    if(MySC==MyED){
        QMessageBox::critical(this,"错误！","出发城市和终点城市重复！");
        return false;
    }
    //防止列车班次重复：数据有效性的检测
    bool found=false;//找到与否
    //创立查询
    QSqlQuery* query=this->data->getNewQuery();
    query->exec(QString("select * from trainclass where TrainClass='%1'").arg(train->trainclass.toLower()));
    found=query->next();//更新found的值
    delete query;
    if(found) QMessageBox::critical(this,"错误！","列车班次重复！");
    return !found;
}

//修改列车
void TrainManage::setData(Train *train, int index){
    QSqlRecord record=model->record();
    record.setValue("TrainClass",train->trainclass);
    record.setValue("StartDateTime",train->StartDT.toString("yyyy-MM-dd HH:mm"));
    record.setValue("StartCity",train->StartCity);
    record.setValue("EndCity",train->EndCity);
    record.setValue("CarNum",train->CarNum);
    record.setValue("SeatNum",train->SeatNum);
    if(index==-1){//添加时
        model->insertRecord(model->rowCount(),record);//插入数据
    }
    else{//修改时
        model->setRecord(index,record);//设置数据
    }
    model->submitAll();
    model->select();//刷新
}
//创建表
void TrainManage::CreateTrainTable(Train* train){
    QSqlQuery* query=this->data->getNewQuery();
    //设置指令字符串
    QString exe=QString("CREATE TABLE IF NOT EXISTS %1(").arg(train->trainclass.toLower());
    exe.append("Psgid char(22) NOT NULL, Cars int NOT NULL, Seats int NOT NULL, FromCity int NOT NULL, ToCity int NOT NULL)");
    qDebug()<<exe;
    if(!query->exec(exe)){//失败
        qDebug("unable to execute the table-creating exp");
        delete query;
        return;
    }
    QString exe2=QString("CREATE TABLE IF NOT EXISTS %1(indexNum int, City char(22))").
            arg(QString("transfer_").append(train->trainclass.toLower()));
    qDebug()<<exe2;
    if(!query->exec(exe2)){//失败
        qDebug("unable to execute the table-creating exp2");
        delete query;
        return;
    }
    //插入车站

    QString exe3=QString("INSERT INTO transfer_%1 VALUES(%2,'%3')")
            .arg(train->trainclass.toLower()).arg(0).arg(train->StartCity);
    qDebug()<<exe3;
    if(!query->exec(exe3)){//失败
        qDebug("unable to execute the table-creating exp3");
        delete query;
        return;
    }

    QString exe4=QString("INSERT INTO transfer_%1 VALUES(%2,'%3')")
            .arg(train->trainclass.toLower()).arg(-1).arg(train->EndCity);
    qDebug()<<exe4;
    if(!query->exec(exe4)){//失败
        qDebug("unable to execute the table-creating exp4");
        delete query;
        return;
    }
    delete query;
}

//修改表
bool TrainManage::EditTrainTable(Train *train,const QList<QVariant>& list){
    QSqlQuery *query=data->getNewQuery();
    const QString MyTrainClass=train->trainclass.toLower();
    const QString MySC=train->StartCity;
    const QString MyED=train->EndCity;
    if(MyTrainClass!=list[TrainClassIndex].toString().toLower()){//如果列车名改了
        QString tmpexe=QString("ALTER TABLE %1 RENAME TO %2")
                .arg(list[TrainClassIndex].toString().toLower()).arg(MyTrainClass);
        qDebug()<<tmpexe;
        if(query->exec(tmpexe)){
            qDebug()<<"successfully altered train name";
        }
        else{
            delete query;
            throw std::invalid_argument("unable to alter train name!");
            return false;
        }
        QString tmpexe2=QString("ALTER TABLE %1 RENAME TO %2")
                .arg(QString("transfer_").append(list[TrainClassIndex].toString().toLower()))
                .arg(QString("transfer_").append(MyTrainClass));
        qDebug()<<tmpexe2;
        if(query->exec(tmpexe2)){
            qDebug()<<"successfully altered train name";
        }
        else{
            delete query;
            throw std::invalid_argument("unable to alter train name!");
            return false;
        }
    }
    //直接更改发车和终点城市
    QString tmpexe3=QString("UPDATE transfer_%1 SET City= '%2' WHERE indexNum=0")
            .arg(MyTrainClass).arg(MySC);
    QString tmpexe4=QString("UPDATE transfer_%1 SET City= '%2' WHERE indexNum=-1")
            .arg(MyTrainClass).arg(MyED);
    qDebug()<<tmpexe3;
    if(query->exec(tmpexe3)){
        qDebug()<<"successfully altered sc";
    }
    else{
        delete query;
        throw std::invalid_argument("unable to alter sc");
        return false;
    }
    qDebug()<<tmpexe4;
    if(query->exec(tmpexe4)){
        qDebug()<<"successfully altered ed";
    }
    else{
        delete query;
        throw std::invalid_argument("unable to alter ed");
        return false;
    }
    return 1;
}

//检测列车是否为空列车
bool TrainManage::isTrainEmpty(const QString& trainclass){
    QSqlQuery *query=data->getNewQuery();
    QString tmpexe=QString("SELECT * FROM %1 WHERE PsgID IS NOT NULL").arg(trainclass.toLower());
    bool re=query->exec(tmpexe);
    if(!re){
        qDebug("unable to select null cars from table");
        QMessageBox::critical(this,"错误！","系统故障！unable to select null cars from table");
        delete query;
        throw std::invalid_argument("");
        return 0;
    }
    if(query->next()){
        qDebug()<<"not empty seat: "<<query->value(1)<<query->value(2);
        delete query;
        return false;
    }
    delete query;
    return true;
}


void TrainManage::on_AddBtn_clicked()//添加
{
    //创立编辑窗口，并创立Train*的指针
    Train *train=0;
    TrainEditDialog *dlg=new TrainEditDialog(train);
    dlg->SetTitle("添加班次");
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->exec();
    if(!train){
        qDebug("没有添加列车");
        return;
    }
    if(!isDataValid(train)){
        delete train;
        emit this->ui->AddBtn->clicked();
        return;
    }
    //如果没找到，说明数据有效
    CreateTrainTable(train);//生成新的列车表
    setData(train);//设置表格和主表
    delete train;
    //this->data->query.exec("")
    qDebug("添加列车成功！");
    QMessageBox::information(this,"提示","添加列车成功");

}

void TrainManage::on_DeleteBtn_clicked()//删除
{
    QItemSelectionModel *selectionModel = ui->tableView->selectionModel();
    QModelIndexList selectedRows = selectionModel->selectedRows();
    if (selectedRows.isEmpty()) {
           QMessageBox::information(ui->tableView, "提示", "请选择至少一个班次信息");
           return;
     }
    if(QMessageBox::Yes==QMessageBox::warning(this,"提示","您确定要删除该班次信息吗？\n若该班次有乘客购票，将被强制退票",
                                              QMessageBox::Yes|QMessageBox::No,QMessageBox::No))
    {
        QSqlQuery* query=data->getNewQuery();
        for (const QModelIndex &index : selectedRows) {
            //获取行数
            int row = index.row();
            //获取班次号，即表名
            QString trainclass=model->data(model->index(row,TrainClassIndex)).toString();
            //执行
            bool re=query->exec(QString("DROP TABLE IF EXISTS %1").arg(trainclass));//删除主表
            bool re_2=query->exec(QString("DROP TABLE IF EXISTS %1").arg(QString("transfer_").append(trainclass)));//删除中转站表
            if(!re||!re_2){
                qDebug("Error! Failed to drop table");
                QMessageBox::critical(this,"错误","系统故障！Failed to drop table");
                delete query;
                delete this;
                return;
            }
            model->removeRow(row); // 删除选中的行
        }
           model->select();//刷新
           delete query;//释放空间
           QMessageBox::information(this,"提示","删除班次成功");
    }
}

void TrainManage::on_EditBtn_clicked()//修改
{
    //获取当前的行数
    int index=this->ui->tableView->currentIndex().row();
    if(index<0){//没有选择
        QMessageBox::information(ui->tableView, "提示", "请选择至少一个列车班次");
        return;
    }

    QList<QVariant> datalist;//初始数据
    for(int i=0;i<TrainParaNum;i++){
        datalist.append(model->data(model->index(index,i),Qt::DisplayRole));
    }
    qDebug()<<datalist;
    //创立编辑窗口，并创立Train*的指针
    Train *train=0;
    TrainEditDialog *dlg=new TrainEditDialog(datalist,train);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->SetTitle("修改班次");
    dlg->exec();
    if(!train){//如果为空
        qDebug("没有修改列车");
        return;
    }
    try{
    if(!isDataValid(train,index)){
        delete train;
        emit this->ui->EditBtn->clicked();
        return;
    }
    }
    catch(std::invalid_argument&exp){
        delete train;
        delete this;
        return;//崩溃，退出
    }
    //如果没找到，说明数据有效
    try{
        EditTrainTable(train,datalist);
    }
    catch(std::invalid_argument& exp){
        qDebug()<<exp.what();
        QMessageBox::critical(this,"错误",QString("系统故障！").append(exp.what()));
        delete train;
        delete this;
        return;//崩溃，退出
    }
    setData(train,index);
    delete train;
    qDebug("修改列车成功！");
    QMessageBox::information(ui->tableView, "提示", "修改班次成功");
}

void TrainManage::on_EditTransBtn_clicked()//修改中间站
{
    int index=this->ui->tableView->currentIndex().row();
    if(index<0){//没有选择
        QMessageBox::information(ui->tableView, "提示", "请选择至少一个列车班次");
        return;
    }
    //检测是否为空车
    const QString MyTrainClass=model->data(model->index(index,TrainClassIndex)).toString();
    if(!isTrainEmpty(MyTrainClass)){
        qDebug("not empty train");
        QMessageBox::information(this,"提示","非空列车不允许调整中间站！");
        return;//表示无效
    }
    edit=new EditTransfer(this->data,MyTrainClass);
    edit->setAttribute(Qt::WA_DeleteOnClose);
    edit->exec();

}
