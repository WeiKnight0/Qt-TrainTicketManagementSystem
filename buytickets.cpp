#include "buytickets.h"
#include "ui_buytickets.h"
#include<QMessageBox>
#define TrainParaNum (6)

BuyTickets::BuyTickets(DataSystem* data_input,QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::BuyTickets),
    data(data_input),
    startlist(0),
    endlist(0)
{
    ui->setupUi(this);
    QStringList list(QString("列车班次#发车时间#发车城市#终点城市#车厢数#每厢座位数#余票数").split("#"));
    ui->traintableWidget->setColumnCount(7);
    ui->traintableWidget->setHorizontalHeaderLabels(list);
    ui->traintableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//不可修改
    ui->traintableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);//选择行
    ui->traintableWidget->setSelectionMode(QAbstractItemView::SingleSelection);//只能选择一行
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
    //设置证件号只能输入字母和数字
    ui->IDEdit->setAttribute(Qt::WA_InputMethodEnabled, false);
    //设置电话号码只能输入数字
    QValidator *validator=new QIntValidator(ui->PhoneEdit);
    ui->PhoneEdit->setAttribute(Qt::WA_InputMethodEnabled, false);
    ui->PhoneEdit->setValidator(validator);
    //设置初始状态为不自选
    emit ui->checkBox->stateChanged(0);
    ui->checkBox->setChecked(false);
    //获取所有乘客
    QSqlQuery *query=data->getNewQuery();
    bool re=query->exec("SELECT * FROM passengers");
    if(!re)
    {
        delete query;
        QMessageBox::critical(this,"错误！","系统故障！failed to select passengers");
        delete this;
        return;
    }
    while(query->next()){
        ui->psgtableWidget->insertRow(ui->psgtableWidget->rowCount());
        for(int i=0;i<6;i++){
            QTableWidgetItem *item=new QTableWidgetItem;
            item->setData(Qt::DisplayRole,query->value(i));
            ui->psgtableWidget->setItem(ui->psgtableWidget->rowCount()-1,i,item);
        }
    }
    delete query;
}

BuyTickets::~BuyTickets()
{
    delete ui;
    deleteptr();
}

void BuyTickets::on_searchBtn_clicked()
{
    if(ui->StartEdit->text().trimmed().isEmpty()||ui->EndEdit->text().trimmed().isEmpty()){
        QMessageBox::information(this,"提示","请输入出发站点和终到站点");
        return;
    }
    deleteptr();
    for (int i = ui->traintableWidget->rowCount()- 1; i >= 0; --i) {
        ui->traintableWidget->removeRow(i);
    }
    //存储城市
    this->start=ui->StartEdit->text().trimmed();
    this->end=ui->EndEdit->text().trimmed();
    //获取所有的列车号的list
    QStringList trainlist=data->getAllTrains();
    qDebug()<<trainlist;
    int SearchCount=0;
    startlist=new QList<int>;
    endlist=new QList<int>;
    for(QString trainclass:trainlist){
        int prev=-2;//前
        int post=-2;//后
        //查找出发城市
        prev=data->getCityIndex(trainclass,ui->StartEdit->text().trimmed());
        //查找终到城市
        post=data->getCityIndex(trainclass,ui->EndEdit->text().trimmed());
        if(prev==-3||post==-3){//故障
            delete this;
            return;
        }
        qDebug()<<trainclass<<' '<<prev<<' '<<post;
        if(prev>=0&&prev!=post&&(prev<post||post==-1)){//符合先后顺序
            qDebug()<<"found";
            //存储序号
            startlist->append(prev);
            endlist->append(post);
            //显示该列车
            QSqlQuery* query=data->getNewQuery();
            if(!query->exec(QString("SELECT * FROM trainclass WHERE TrainClass='%1'").arg(trainclass))){
                qDebug()<<"failed to select trains";
                delete query;
                QMessageBox::critical(this,"错误！","系统故障！failed to select trains");
                delete this;
                return;
            }
            while(query->next()){
                ++SearchCount;
                ui->traintableWidget->insertRow(ui->traintableWidget->rowCount());
                for(int i=0;i<6;i++){
                    QTableWidgetItem *item=new QTableWidgetItem;
                    item->setData(Qt::DisplayRole,query->value(i));
                    ui->traintableWidget->setItem(ui->traintableWidget->rowCount()-1,i,item);
                }
                //余票数
                QTableWidgetItem *item=new QTableWidgetItem;
                item->setData(Qt::DisplayRole,data->getRemainingTickets(trainclass,prev,post));
                ui->traintableWidget->setItem(ui->traintableWidget->rowCount()-1,6,item);
            }
            delete query;
        }
    }
    if(SearchCount){
        QMessageBox::information(this,"提示",QString("共找到 %1 个班次信息！").arg(SearchCount));
    }
    else{
        QMessageBox::information(this,"提示","没有找到任何班次信息！");
    }
}

void BuyTickets::on_buyBtn_clicked()
{
    int row=ui->traintableWidget->currentRow();
    if(row==-1){
        qDebug()<<"没有选择";
        QMessageBox::information(this,"提示","请选择一个列车班次");
        return;
    }
    if(ui->traintableWidget->item(row,6)->text().toInt()==0){
        QMessageBox::information(this,"提示","该列车已售罄");
        return;
    }
    this->trainclass=ui->traintableWidget->item(row,0)->text();
    this->carnum=ui->traintableWidget->item(row,4)->text().toInt();
    this->seatnum=ui->traintableWidget->item(row,5)->text().toInt();
    ui->carBox->setMaximum(this->carnum);
    ui->seatBox->setMaximum(this->seatnum);
    ui->clsLabel->setText(this->trainclass);
    ui->sLabel->setText(this->start);
    ui->eLabel->setText(this->end);
    qDebug()<<"row="<<row;
    this->startindex=startlist->at(row);
    this->endindex=endlist->at(row);
    qDebug()<<"choosed:"<<trainclass<<startindex<<endindex;
    ui->stackedWidget->setCurrentIndex(1);
}

void BuyTickets::on_cancelBtn_clicked()
{
    if(QMessageBox::Yes==QMessageBox::question(this,"提示","您确定要退出吗？")){
        this->close();
    }
}

void BuyTickets::on_toPrevBtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(ui->stackedWidget->currentIndex()-1);
}

void BuyTickets::on_cancelBtn_2_clicked()
{
    emit ui->cancelBtn->clicked();
}



void BuyTickets::on_confirmBtn_clicked(){

    int row=this->ui->psgtableWidget->currentIndex().row();
    if(row<0){
        QMessageBox::information(this, "提示", "请选择一名乘客");
        return;
    }
    QList<QTableWidgetItem*> selecteditems=ui->psgtableWidget->selectedItems();
    QString name=selecteditems[0]->text();
    QString id=selecteditems[3]->text();
    QString ph=selecteditems[5]->text();
    //检测是否已经购票
    if(data->haveBoughtTickets(id,this->trainclass)){
        QMessageBox::critical(this,"提示","该乘客已经在该列车购买了票");
        return;
    }
    QString info=QString("确定为乘客 %1 (证件号：%2) 购买 %3 次列车吗？").arg(name).arg(id).arg(this->trainclass);
    if(QMessageBox::Yes==QMessageBox::question(this,"提示",info)){
        if(ui->checkBox->isChecked()){//自选
            QSqlQuery*query=data->getNewQuery();
            bool ok=true;//是否可以购买
            QString exe=QString("SELECT * FROM %1 WHERE Cars=%2 AND Seats=%3")
                    .arg(this->trainclass.toLower()).arg(ui->carBox->value())
                    .arg(ui->seatBox->value());
            qDebug()<<exe;
            if(!query->exec(exe)){
                QMessageBox::critical(this,"错误！","系统故障！failed to select trains");
                delete this;
                return;
            }
            while(query->next()){
                int start=query->value(3).toInt();//起始
                int end=query->value(4).toInt();//终到
                if(!((end!=-1&&end<=this->startindex)||(start>=this->endindex&&this->endindex!=-1))){
                    ok=false;
                }
            }
            if(ok){
                qDebug()<<"可以购买";
                this->name=name;
                this->id=id;
                this->phone=ph;
                this->car=ui->carBox->value();
                this->seat=ui->seatBox->value();
                ui->nameLabel->setText(this->name);
                ui->idLabel->setText(this->id);
                ui->phLabel->setText(this->phone);
                ui->carLabel->setText(QString::number(car));
                ui->seatLabel->setText(QString::number(seat));
                ui->stackedWidget->setCurrentIndex(2);
            }
            else{
                qDebug()<<"不能购买";
                QMessageBox::information(this,"提示","该座位已售出！请重新选择！");
                delete query;
                return;
            }
            delete query;
        }
        else{//系统安排
            for(int i=1;i<=carnum;i++){
                for(int j=1;j<=seatnum;j++){
                    QSqlQuery*query=data->getNewQuery();
                    bool ok=true;//是否可以购买
                    QString exe=QString("SELECT * FROM %1 WHERE Cars=%2 AND Seats=%3")
                            .arg(this->trainclass.toLower()).arg(i)
                            .arg(j);
                    qDebug()<<exe;
                    if(!query->exec(exe)){
                        qDebug()<<"无法搜索！";
                        delete this;
                        return;
                    }
                    while(query->next()){
                        int start=query->value(3).toInt();//起始
                        int end=query->value(4).toInt();//终到
                        if(!((end!=-1&&end<=this->startindex)||(start>=this->endindex&&this->endindex!=-1))){
                            ok=false;
                        }
                    }
                    if(ok){
                        qDebug()<<"可以购买"<<i<<j;
                        this->name=name;
                        this->id=id;
                        this->phone=ph;
                        this->car=i;
                        this->seat=j;
                        ui->nameLabel->setText(this->name);
                        ui->idLabel->setText(this->id);
                        ui->phLabel->setText(this->phone);
                        ui->carLabel->setText(QString::number(i));
                        ui->seatLabel->setText(QString::number(j));
                        ui->stackedWidget->setCurrentIndex(2);
                        delete query;
                        return;
                    }
                    else{
                        qDebug()<<"不能购买"<<i<<j;
                        delete query;
                        continue;
                    }
                }
            }
            QMessageBox::critical(this,"错误","该班次无法购买！");//遍历后也没有票
        }
    }
}

void BuyTickets::on_checkBox_stateChanged(int arg1)
{
    if(arg1==0){
        ui->carBox->setEnabled(false);
        ui->seatBox->setEnabled(false);
    }
    if(arg1==2){
        ui->carBox->setEnabled(1);
        ui->seatBox->setEnabled(1);
    }
}

void BuyTickets::on_searchBtn_2_clicked()
{
    QSqlQuery* query=data->getNewQuery();
    if(ui->NameEdit->text().isEmpty()||ui->IDEdit->text().isEmpty()||ui->PhoneEdit->text().isEmpty()){
        QMessageBox::information(this,"提示","请输入姓名、证件号和电话号码");
        return;
    }
    QString exe= QString("SELECT * FROM passengers WHERE Name='%1' AND ID='%2' AND Phone='%3'")
            .arg(ui->NameEdit->text()).arg(ui->IDEdit->text()).arg(ui->PhoneEdit->text());
    qDebug()<<exe;
    if(!query->exec(exe)){
        qDebug()<<"failed to select passengers";
        QMessageBox::critical(this,"错误！","系统故障！failed to select passengers");
        delete this;
        return;
    }
    for (int i = ui->psgtableWidget->rowCount()- 1; i >= 0; --i) {
        ui->psgtableWidget->removeRow(i);
    }
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

void BuyTickets::on_toPrevBtn_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void BuyTickets::on_cancelBtn_3_clicked()
{
    emit ui->cancelBtn->clicked();
}

void BuyTickets::on_confirmBtn_2_clicked()
{
    //添加数据
    QSqlQuery* query=data->getNewQuery();
    QString exe=QString("INSERT INTO %1 VALUES('%2',%3,%4,%5,%6) ")
            .arg(this->trainclass.toLower()).arg(this->id).arg(this->car)
            .arg(this->seat).arg(this->startindex).arg(this->endindex);
    if(!query->exec(exe)){
        qDebug()<<"失败";
    }
    qDebug()<<"购票成功";
    QMessageBox::information(this,"提示","购票成功！");
    delete query;
    this->close();
}
