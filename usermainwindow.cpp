#include "usermainwindow.h"
#include "ui_usermainwindow.h"
#include <QMessageBox>

userMainWindow::userMainWindow(DataSystem*input,Login* lg,const QString& str,QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::userMainWindow),
    data(input),
    login(lg)
{
    ui->setupUi(this);
    setWindowTitle("列车售票系统——乘客端");
    ui->label_tt->setText(QString(QString("欢迎您，用户 %1 ！\n"
                                          "请选择下方的服务类型").arg(str)));
    ui->actionShow->setText(QString("已登录用户： %1").arg(str));
    //获取id
    QSqlQuery* query=data->getNewQuery();
    if(!query->exec(QString("SELECT relatedID FROM users WHERE account='%1'").arg(str))){
        qDebug()<<"错误！";
        delete query;
        QMessageBox::critical(this,"错误！","系统故障！failed to select related id");
        delete this;
        return;
    }
    while(query->next()){
        this->id=query->value(0).toString();
    }
    //修改密码
    connect(ui->actionChangepsw,&QAction::triggered,[=](){
        QStringList* list=0;
        ChangePsw* dlg=new ChangePsw(list,str);
        dlg->setAttribute(Qt::WA_DeleteOnClose);
        dlg->exec();
        if(!list){
            qDebug()<<"没有改密码";
            return;
        }
        QSqlQuery* query=data->getNewQuery();
        if(!query->exec(QString("SELECT * FROM users WHERE relatedID='%1'").arg(id))){
            delete query;
            QMessageBox::critical(this,"错误！","系统故障！failed to select user");
            delete this;
            return;
        }
        while(query->next()){
            QString psc=query->value(1).toString();
            if(psc!=list->at(0)){//密码错误
                qDebug()<<"old psc wrong"<<psc<<list->at(0);
                QMessageBox::critical(this,"错误","原密码错误！");
                return;
            }
            //修改密码
            if(!query->exec(QString("UPDATE users SET passcode='%1' WHERE relatedID='%2'")
                            .arg(list->at(1)).arg(id)))
            {
                delete query;
                QMessageBox::critical(this,"错误！","系统故障！failed to update psw");
                delete this;
                return;
            }
            QMessageBox::information(this,"提示","修改成功！");
            delete query;
        }
    });

    connect(ui->actionBuy,&QAction::triggered,ui->buyBtn,&QPushButton::clicked);
    connect(ui->actionReturn,&QAction::triggered,ui->returnBtn,&QPushButton::clicked);
    connect(ui->actionLogout,&QAction::triggered,[=](){
        this->close();
    });
    connect(ui->actionAbout,&QAction::triggered,[=](){
        AboutDialog dlg(this);
        dlg.exec();
    });
}

userMainWindow::~userMainWindow()
{
    delete ui;
}

void userMainWindow::closeEvent(QCloseEvent *event){
        // 弹出确认对话框
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "提示", "您确认要退出登录吗",
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

void userMainWindow::on_buyBtn_clicked()
{
    userBuy* userbuy=new userBuy(data,id,this);
    userbuy->setWindowModality(Qt::WindowModal);
    userbuy->setAttribute(Qt::WA_DeleteOnClose);//关闭时销毁
    userbuy->show();
}

void userMainWindow::on_returnBtn_clicked()
{
    userReturn* userrt=new userReturn(data,id,this);
    userrt->setWindowModality(Qt::WindowModal);
    userrt->setAttribute(Qt::WA_DeleteOnClose);//关闭时销毁
    userrt->show();
}
