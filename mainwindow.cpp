#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QAction>
#include<QMessageBox>


MainWindow::MainWindow(DataSystem* data_input,const QString& str,Login* ptr,QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    data(data_input),
    adminname(str),
    login(ptr)
{
    ui->setupUi(this);
    ui->actionShow->setText(QString("已登录账号：").append(adminname));
    //更改密码
    connect(ui->actionChangepsw,&QAction::triggered,[=](){
        QStringList* list=0;
        ChangePsw* dlg=new ChangePsw(list,adminname);
        dlg->setAttribute(Qt::WA_DeleteOnClose);
        dlg->exec();
        if(!list){
            qDebug()<<"没有改密码";
            return;
        }
        QSqlQuery* query=data->getNewQuery();
        if(!query->exec(QString("SELECT * FROM admin WHERE account='%1'").arg(adminname))){

        }
        while(query->next()){
            QString psc=query->value(1).toString();
            if(psc!=list->at(0)){//密码错误
                qDebug()<<"old psc wrong"<<psc<<list->at(0);
                QMessageBox::critical(this,"错误","原密码错误！");
                return;
            }
            //修改密码
            if(!query->exec(QString("UPDATE admin SET passcode='%1' WHERE account='%2'")
                            .arg(list->at(1)).arg(adminname)))
            {

            }
            QMessageBox::information(this,"提示","修改成功！");
        }
    });
    //退出登录
    connect(ui->actionLogout,&QAction::triggered,[=](){
        this->close();
    });
    //连接菜单和按钮信号
    connect(ui->actionTrain,&QAction::triggered,ui->TrainBtn,&QPushButton::clicked);
    connect(ui->actionSearch,&QAction::triggered,ui->SearchBtn,&QPushButton::clicked);
    connect(ui->actionPassengers,&QAction::triggered,ui->PassengerBtn,&QPushButton::clicked);
    connect(ui->actionBuy,&QAction::triggered,ui->BuyBtn,&QPushButton::clicked);
    connect(ui->actionReturn,&QAction::triggered,ui->ReturnBtn,&QPushButton::clicked);
    connect(ui->actionFile,&QAction::triggered,ui->FileBtn,&QPushButton::clicked);
    connect(ui->actionAbout,&QAction::triggered,[=](){
        AboutDialog dlg(this);
        dlg.exec();
    });

}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::closeEvent(QCloseEvent *event){
        // 弹出确认对话框
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "提示", "您确定要退出登录吗",
                                      QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            // 如果用户选择“是”，允许窗口关闭
            login->show();
            event->accept();
        } else {
            // 如果用户选择“否”，取消窗口关闭
            event->ignore();
        }
}


void MainWindow::on_TrainBtn_clicked()
{
    TrainManage* trainmanage=new TrainManage(this->data,this);
    trainmanage->setWindowModality(Qt::WindowModal);
    trainmanage->setAttribute(Qt::WA_DeleteOnClose);//关闭时销毁
    trainmanage->show();
}

void MainWindow::on_SearchBtn_clicked()
{
    TrainSearch* trainS=new TrainSearch(this->data);
    trainS->setAttribute(Qt::WA_DeleteOnClose);
    trainS->exec();
}


void MainWindow::on_PassengerBtn_clicked()
{
    PassengerManage* PsgMng= new PassengerManage(this->data,this);
    PsgMng->setWindowModality(Qt::WindowModal);
    PsgMng->setAttribute(Qt::WA_DeleteOnClose);
    PsgMng->show();
}

void MainWindow::on_BuyBtn_clicked()
{
    BuyTickets* buytickets=new BuyTickets(this->data,this);
    buytickets->setWindowModality(Qt::WindowModal);
    buytickets->setAttribute(Qt::WA_DeleteOnClose);
    buytickets->show();
}

void MainWindow::on_ReturnBtn_clicked()
{
    ReturnTickets*rtntickets=new ReturnTickets(this->data,this);
    rtntickets->setWindowModality(Qt::WindowModal);
    rtntickets->setAttribute(Qt::WA_DeleteOnClose);
    rtntickets->show();
}

void MainWindow::on_FileBtn_clicked()
{
    fileWindow* fw=new fileWindow(this->data,this);
    fw->setWindowModality(Qt::WindowModal);
    fw->setAttribute(Qt::WA_DeleteOnClose);
    fw->show();
}
