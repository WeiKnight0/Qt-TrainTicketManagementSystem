#include "changepsw.h"
#include "ui_changepsw.h"
#include<QMessageBox>

ChangePsw::ChangePsw(QStringList*& ptr,const QString& str,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChangePsw),
    storage(ptr)
{
    ui->setupUi(this);
    setWindowFlags(Qt::WindowCloseButtonHint);
    ui->label_tt->setText(QString("修改密码——用户名：").append(str));
    storage=0;
    //设置密码输入模式
     ui->oldEdit->setAttribute(Qt::WA_InputMethodEnabled, false);
     ui->newEdit->setAttribute(Qt::WA_InputMethodEnabled, false);
     ui->newEdit_2->setAttribute(Qt::WA_InputMethodEnabled, false);
}

ChangePsw::~ChangePsw()
{
    delete ui;
}

void ChangePsw::on_cancelBtn_clicked()
{
    if(!storage){
        delete storage;
        storage=0;
    }
    this->close();
}

void ChangePsw::on_confirmBtn_clicked()
{
    if(ui->newEdit->text().trimmed()!=ui->newEdit_2->text().trimmed()){
        QMessageBox::information(this,"提示","两次输入的新密码不一致！");
        return;
    }
    storage=new QStringList;
    storage->append(ui->oldEdit->text().trimmed());
    storage->append(ui->newEdit->text().trimmed());
    this->close();
    return;
}
