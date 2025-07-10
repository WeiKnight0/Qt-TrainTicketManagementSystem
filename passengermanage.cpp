#include "passengermanage.h"
#include "ui_passengermanage.h"
#include"psgeditdialog.h"//乘客修改界面
#include<QMessageBox>

PassengerManage::PassengerManage(DataSystem* input,QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PassengerManage),
    data(input)
{
    ui->setupUi(this);
    model=new QSqlTableModel;
    model->setTable("passengers");
    //设置表头
    QStringList list(QString("姓名#性别#生日#证件号#国家/地区#电话号码").split("#"));
    [&](QStringList& mylist)->void{
        for(int i=0;i<PsgParaNum;i++){
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

PassengerManage::~PassengerManage()
{
    delete ui;
}

//数据是否无效
bool PassengerManage::isDataInvalid(Passenger *psg,int index){
    if(!psg)
    {
        qDebug()<<"psg is null!";

        return 0;
    }
    if(index!=-1)//如果是修改
    {
        if(psg->id==model->data(model->index(index,idIndex)).toString()){
            return false;
        }
        if(data->haveBoughtTickets(model->data(model->index(index,idIndex)).toString())){//买了票
            QMessageBox::critical(this,"错误！","乘客已经购票！请退票后再修改证件号！");
            return true;
        }
    }
    bool found=false;//找到与否
    //创立查询
    QSqlQuery* query=this->data->getNewQuery();
    //设置指令并执行
    query->prepare("select * from passengers where ID = :myid ");
    query->bindValue(":myid",psg->id);
    query->exec();
    found=query->next();//判断是否找到
    delete query;//释放空间
    if(found){
        QMessageBox::critical(this,"错误！","乘客证件号重复！请重新添加");
    }
    return found;
}

//添加/修改数据
void PassengerManage::setData(Passenger *psg, int index){
    QSqlRecord record=model->record();
    qDebug("Data is valid!");
    //设置值
    record.setValue("Name",psg->name);
    record.setValue("Gender",psg->gender);
    record.setValue("Birthday",psg->birth);
    record.setValue("ID",psg->id);
    record.setValue("Nationality",psg->nation);
    record.setValue("Phone",psg->phone);
    if(index==-1){//添加时
        model->insertRecord(model->rowCount(),record);
    }
    else{//插入时
        model->setRecord(index,record);
    }
    model->submitAll();
    model->select();//刷新
}

void PassengerManage::on_AddBtn_clicked()
{
    //创立编辑窗口，并创立Passenger*的指针
    Passenger* psg=0;
    PsgEditDialog* dlg=new PsgEditDialog(psg);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->SetTitle("添加乘客");
    dlg->exec();
    if(!psg){
        qDebug("没有添加");
        return;
    }
    if(isDataInvalid(psg)){//如果找到了，说明重复了
        delete psg;
        emit this->ui->AddBtn->clicked();
        return;
    }
    //如果没找到，说明数据有效
    setData(psg);
    delete psg;
    QMessageBox::information(this,"提示","添加乘客成功");
    qDebug("添加乘客成功！");
}

void PassengerManage::on_EditBtn_clicked()
{
    //获取当前的行数
    int index=this->ui->tableView->currentIndex().row();
    if(index<0){
        QMessageBox::information(ui->tableView, "提示", "请选择一行");
        return;
    }
    QList<QVariant> datalist;
    for(int i=0;i<PsgParaNum;i++){//6--总共6列
        datalist.append(model->data(model->index(index,i),Qt::DisplayRole));
    }
    qDebug()<<datalist;
    //创立编辑窗口，并创立Passenger*的指针
    Passenger* psg=0;
    PsgEditDialog* dlg=new PsgEditDialog(psg,datalist);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->SetTitle("修改乘客");
    dlg->exec();
    if(!psg){//如果为空
        qDebug("没有修改乘客");
        return;
    }
    //判断乘客是否已经购票
    if(isDataInvalid(psg,index)){//如果找到了，说明重复了
        delete psg;
        emit this->ui->EditBtn->clicked();
        return;
    }
    //如果没找到，说明数据有效
    setData(psg,index);
    delete psg;
    qDebug("修改乘客成功！");
    QMessageBox::information(this,"提示","修改乘客成功");
}

void PassengerManage::on_DeleteBtn_clicked()
{
    QItemSelectionModel *selectionModel = ui->tableView->selectionModel();
    QModelIndexList selectedRows = selectionModel->selectedRows();

    if (selectedRows.isEmpty()) {
           QMessageBox::information(ui->tableView, "提示", "请选择一名乘客的数据");
           return;
     }

    if(QMessageBox::Yes==QMessageBox::question(this,"提示","您确定要删除该乘客的信息吗？\n"
                                               "若乘客绑定了用户，则该用户亦将被注销账号")){
        qDebug("YES");
        QSqlQuery*query=data->getNewQuery();
        for (const QModelIndex &index : selectedRows) {
               int row = index.row();
               QString id=model->data(model->index(row,3)).toString();
               if(data->haveBoughtTickets(id)){
                   delete query;
                    QMessageBox::critical(this,"错误！","乘客已经购票！无法删除！");
                    return;
               }
               //删除相关联用户
               if(!query->exec(QString("DELETE FROM users WHERE relatedID='%1'").arg(id))){
                   qDebug()<<"错误！";
                   delete query;
                   return;
               }
               model->removeRow(row); // 删除选中的行
           }
           model->select();//刷新
           QMessageBox::information(this,"提示","删除乘客成功");
           delete query;
    }
}
