#include "login.h"
#include "ui_login.h"
#include<QMessageBox>
#include<QDebug>
#include"mainwindow.h"
#include"usermainwindow.h"
#include"userregister.h"
#include"configdatabase.h"


Login::Login(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Login),
    data(new DataSystem)
{
    ui->setupUi(this);
    setWindowFlags(Qt::WindowCloseButtonHint);
    //数据库显示
    UpdateDBShow();
    //转化
    connect(ui->users,&QRadioButton::toggled,[=](bool x){
        if(x){
            ui->stackedWidget->setCurrentIndex(0);
        }
    });
    connect(ui->admin,&QRadioButton::toggled,[=](bool x){
        if(x){
            ui->stackedWidget->setCurrentIndex(1);
        }
    });
    connect(ui->actionConfigdata,&QAction::triggered,[&](){
        ConfigDatabase* config=new ConfigDatabase(this->data);
        config->setAttribute(Qt::WA_DeleteOnClose);
        config->exec();
        UpdateDBShow();
    });

    ui->stackedWidget->setCurrentIndex(0);
    ui->users->setChecked(true);
    //设置用户名只能是字母和数字
    ui->usnLine->setAttribute(Qt::WA_InputMethodEnabled, false);
    //设置密码输入
    ui->pswLine->setAttribute(Qt::WA_InputMethodEnabled, false);
    //关于
    connect(ui->actionAbout,&QAction::triggered,[=](){
        AboutDialog dlg(this);
        dlg.exec();
    });
}

Login::~Login()
{
    qDebug()<<"退出了！";
    delete ui;
    delete data;
}
void Login::UpdateDBShow(){
    if(!data->CheckConnection()){
        ui->actionShow->setText("当前无数据库连接");
    }
    else if(data->getDriverName()=="QODBC"){
        ui->actionShow->setText("已连接到ODBC/MySQL数据库");
    }
    else if(data->getDriverName()=="QSQLITE"){
        ui->actionShow->setText("已连接到SQLite数据库");
    }
}

//退出按钮
void Login::on_exitBtn_clicked()
{
    if(QMessageBox::Yes==QMessageBox::question(this,"提示","您确定要退出吗")){
        this->close();
    }
}

void Login::on_exitBtn_4_clicked()
{
    emit ui->exitBtn->clicked();
}

void Login::on_loginBtn_4_clicked()
{
    //管理员登录
    if(!data->CheckConnection()){
        QMessageBox::critical(nullptr,"错误！","数据库连接失败！！");
        return;
    }
    QSqlQuery* query=data->getNewQuery();
    QString exe=QString("SELECT * FROM admin");
    if(!query->exec(exe)){
        QMessageBox::critical(this,"错误","无法登录！unable to select admin");
        delete query;
        return;
    }
    while(query->next()){
        if(ui->usnLine->text()==query->value(0).toString()
                &&ui->pswLine->text()==query->value(1).toString()){
            qDebug()<<"欢迎您，"<<ui->usnLine->text()<<"！";
            QMessageBox::information(this,"登录成功！",QString("欢迎登录，管理员 ").append(ui->usnLine->text()));
            delete query;
            ui->pswLine->setText(QString());
            MainWindow* mw= new MainWindow(data,ui->usnLine->text(),this);
            mw->setAttribute(Qt::WA_DeleteOnClose);
            mw->show();
            this->close();
            return;
        }
    }
    qDebug()<<"登陆失败！";
    QMessageBox::information(this,"登录失败！",QString("用户名或密码错误"));
    delete query;
}

void Login::on_loginBtn_clicked()
{
    //用户登录
    if(!data->CheckConnection()){
        QMessageBox::critical(nullptr,"错误！","数据库连接失败！！");
        return;
    }
    QSqlQuery* query=data->getNewQuery();
    QString exe=QString("SELECT * FROM users");
    if(!query->exec(exe)){
        QMessageBox::critical(this,"错误","无法登录！unable to select users");
        delete query;
        return;
    }
    while(query->next()){
        if(ui->usnLine->text()==query->value(0).toString()
                &&ui->pswLine->text()==query->value(1).toString()){
            qDebug()<<"欢迎您用户，"<<ui->usnLine->text()<<"！";
            QMessageBox::information(this,"登录成功！",QString("欢迎登录，用户 ").append(ui->usnLine->text()));
            ui->pswLine->setText(QString());
            userMainWindow* umw=new userMainWindow(data,this,ui->usnLine->text());
            umw->setAttribute(Qt::WA_DeleteOnClose);
            umw->show();
            delete query;
            this->close();
            return;
        }
    }
    qDebug()<<"登陆失败！";
    QMessageBox::information(this,"登录失败！",QString("用户名或密码错误"));
    delete query;
}

void Login::on_registerBtn_clicked()
{
    userRegister* ur=new userRegister(data);
    ur->setAttribute(Qt::WA_DeleteOnClose);
    ur->exec();
}
