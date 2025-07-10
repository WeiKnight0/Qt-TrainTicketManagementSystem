#include "userbuy.h"
#include "ui_userbuy.h"
#include<QMessageBox>

userBuy::userBuy(DataSystem*input,const QString& str,QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::userBuy),
    data(input),
    startlist(0),
    endlist(0),
    selfid(str)
{
    ui->setupUi(this);
    QStringList list(QString("列车班次#发车时间#发车城市#终点城市#车厢数#每厢座位数#余票数").split("#"));
    ui->traintableWidget->setColumnCount(7);
    ui->traintableWidget->setHorizontalHeaderLabels(list);
    ui->traintableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//不可修改
    ui->traintableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);//选择行
    ui->traintableWidget->setSelectionMode(QAbstractItemView::SingleSelection);//只能选择一行
//    //设置stackView初始index
    ui->stackedWidget->setCurrentIndex(0);
    //获取姓名和电话号码
    QSqlQuery* query=data->getNewQuery();
    if(!query->exec(QString("SELECT Name,Phone FROM passengers WHERE ID='%1' ").arg(selfid))){
        delete query;
        QMessageBox::critical(this,"错误！","系统故障！failed to select passengers");
        delete this;
        return;
    }
    while(query->next()){
        this->selfname=query->value(0).toString();
        this->selfphone=query->value(1).toString();
        ui->nameidLabel->setText(QString("%1 (证件号：%2)")
                                 .arg(selfname).arg(selfid));
    }
    delete query;
    //改变为自己以及为他人
    connect(ui->forSelfRB,&QRadioButton::toggled,[=](bool x){
        if(x){
            ui->forSelfWidget->setEnabled(true);
            ui->forOtherWidget->setEnabled(false);
        }
    });
    connect(ui->forOtherRB,&QRadioButton::toggled,[=](bool x){
        if(x){
            ui->forOtherWidget->setEnabled(true);
            ui->forSelfWidget->setEnabled(false);
        }
    });
    //设置输入模式
    ui->NameEdit->setAttribute(Qt::WA_InputMethodEnabled, false);
    ui->IDEdit->setAttribute(Qt::WA_InputMethodEnabled, false);
    QValidator *validator=new QIntValidator(ui->PhoneEdit);
    ui->PhoneEdit->setAttribute(Qt::WA_InputMethodEnabled, false);
    ui->PhoneEdit->setValidator(validator);
}

userBuy::~userBuy()
{
    delete ui;
}

void userBuy::on_searchBtn_clicked()
{
    if(ui->StartEdit->text().trimmed().isEmpty()||ui->EndEdit->text().trimmed().isEmpty()){
        QMessageBox::information(this,"提示","出发站和终到站不得为空！");
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
        QSqlQuery*query1=data->getNewQuery();
        QSqlQuery*query2=data->getNewQuery();
        //查找出发城市
        if(!query1->exec(QString("SELECT * FROM transfer_%1 WHERE City='%2' ")
                        .arg(trainclass).arg(ui->StartEdit->text().trimmed())))
        {
            delete query1;
            QMessageBox::critical(this,"错误！","系统故障！failed to select start city");
            delete this;
            return;
        }//查找失败
        if(query1->next()){prev=query1->value(0).toInt();}
        //查找终到城市
        if(!query2->exec(QString("SELECT * FROM transfer_%1 WHERE City='%2' ")
                            .arg(trainclass).arg(ui->EndEdit->text().trimmed())))
        {
            delete query2;
            QMessageBox::critical(this,"错误！","系统故障！failed to select end city");
            delete this;
            return;
        }//查找失败
        if(query2->next()){post=query2->value(0).toInt();}
        qDebug()<<trainclass<<' '<<prev<<' '<<post;
        if(prev>=0&&prev!=post&&(prev<post||post==-1)){//符合先后顺序
            qDebug()<<"找到的";
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
        delete query1;
        delete query2;
    }
    if(SearchCount){
        QMessageBox::information(this,"提示",QString("共找到 %1 个班次信息！").arg(SearchCount));
    }
    else{
        QMessageBox::information(this,"提示","没有找到任何班次信息！");
    }
}

void userBuy::on_buyBtn_clicked()
{
    int row=ui->traintableWidget->currentRow();
    if(row==-1){
        QMessageBox::information(this,"提示","请至少选择一个列车班次");
        return;
    }
    if(ui->traintableWidget->item(row,6)->text().toInt()==0){
        QMessageBox::information(this,"提示","该列车已售罄");
        return;
    }
    //设置信息
    this->trainclass=ui->traintableWidget->item(row,0)->text();
    this->carnum=ui->traintableWidget->item(row,4)->text().toInt();
    this->seatnum=ui->traintableWidget->item(row,5)->text().toInt();
    //设置显示信息
    ui->trainLabel->setText(this->trainclass.toUpper());
    ui->carBox->setMaximum(this->carnum);
    ui->seatBox->setMaximum(this->seatnum);
    ui->clsLabel->setText(this->trainclass);
    ui->sLabel->setText(this->start);
    ui->eLabel->setText(this->end);
    //设置起点终点信息
    this->startindex=startlist->at(row);
    this->endindex=endlist->at(row);
    qDebug()<<"bought--"<<trainclass<<startindex<<endindex;
    ui->stackedWidget->setCurrentIndex(ui->stackedWidget->currentIndex()+1);
}

void userBuy::on_toPrevBtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}
void userBuy::on_checkBox_stateChanged(int arg1)
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
void userBuy::on_cancelBtn_clicked()
{
    this->close();
}
void userBuy::on_cancelBtn_2_clicked()
{
    emit ui->cancelBtn->clicked();
}
void userBuy::on_cancelBtn_3_clicked()
{
    emit ui->cancelBtn->clicked();
}


void userBuy::on_confirmBtn_clicked()
{
    if(ui->forOtherRB->isChecked()){
        QSqlQuery* query=data->getNewQuery();
        if(ui->NameEdit->text().isEmpty()||ui->IDEdit->text().isEmpty()||ui->PhoneEdit->text().isEmpty()){
            QMessageBox::information(this,"提示","请输入姓名、证件号和电话号码");
            return;
        }
        QString exe= QString("SELECT * FROM passengers WHERE Name='%1' AND ID='%2' AND Phone='%3'")
                .arg(ui->NameEdit->text()).arg(ui->IDEdit->text()).arg(ui->PhoneEdit->text());
        qDebug()<<exe;
        if(!query->exec(exe)){
            qDebug()<<"ERROR";
            QMessageBox::critical(this,"错误！","系统故障！failed to select passengers");
            delete this;
            return;
        }
        if(query->next()){
            qDebug()<<"找到了乘客。。";
            this->name=query->value("Name").toString();
            this->id=query->value("ID").toString();
            this->phone=query->value("Phone").toString();
            delete query;
        }
        else{
            QMessageBox::critical(this,"错误","没有找到这样的乘客！");
            delete query;
            return;
        }
    }
    else if(ui->forSelfRB->isChecked()){
        name=selfname;
        id=selfid;
        phone=selfphone;
    }
    else{
        QMessageBox::information(this,"提示","请至少选择一个乘客选项！");
        return;
    }
    //检测乘客是否已经购票
    if(data->haveBoughtTickets(id,trainclass)){
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
                qDebug()<<"无法搜索！";
                delete query;
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
                this->car=ui->carBox->value();
                this->seat=ui->seatBox->value();
                ui->nameLabel->setText(name);
                ui->phLabel->setText(phone);
                ui->idLabel->setText(id);
                ui->carLabel->setText(QString::number(car));
                ui->seatLabel->setText(QString::number(seat));
                ui->stackedWidget->setCurrentIndex(2);
            }
            else{
                qDebug()<<"不能购买";
                QMessageBox::information(this,"错误","无法购买该座位，请重新选择！");
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
                        delete query;
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
                        qDebug()<<"可以购买"<<i<<j;
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

void userBuy::on_toPrevBtn_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}


void userBuy::on_confirmBtn_2_clicked()
{
    //添加数据
    QSqlQuery* query=data->getNewQuery();
    QString exe=QString("INSERT INTO %1 VALUES('%2',%3,%4,%5,%6) ")
            .arg(this->trainclass.toLower()).arg(this->id).arg(this->car)
            .arg(this->seat).arg(this->startindex).arg(this->endindex);
    if(!query->exec(exe)){
        delete query;
        QMessageBox::critical(this,"错误！","系统故障！failed to insert passenger to train");
        delete this;
        return;
    }
    qDebug()<<"success";
    QMessageBox::information(this,"提示","购票成功！");
    delete query;
    this->close();
}

