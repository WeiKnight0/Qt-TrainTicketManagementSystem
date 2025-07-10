#include "traineditdialog.h"
#include "ui_traineditdialog.h"
#include<QMessageBox>
#include<QDebug>
#include<QRegExpValidator>//正则表达式

TrainEditDialog::TrainEditDialog(Train*& address,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TrainEditDialog),
    train(address)//train是address赋值而来，address是外部指针train的引用
{
    ui->setupUi(this);
    //取消关闭按钮和疑问按钮
    setWindowFlags(windowFlags()&~Qt::WindowCloseButtonHint&~Qt::WindowContextHelpButtonHint);
    //设置班次号只能输入字母和数字
    ui->TrainEdit->setAttribute(Qt::WA_InputMethodEnabled, false);
    //日期时间框显示当前时间
    ui->dtEdit->setDateTime(QDateTime::currentDateTime());
}

TrainEditDialog::~TrainEditDialog()
{
    delete ui;
}

TrainEditDialog::TrainEditDialog(QList<QVariant>& list,Train*& address,QWidget *parent)
    :QDialog(parent),
      ui(new Ui::TrainEditDialog),
      train(address)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags()&~Qt::WindowCloseButtonHint&~Qt::WindowContextHelpButtonHint);
    //填充输入框
    this->ui->TrainEdit->setText(list[0].toString());
    QDateTime dt=QDateTime::fromString(list[1].toString(),"yyyy-MM-dd HH:mm");
    qDebug()<<dt.isValid()<<dt;
    this->ui->dtEdit->setDateTime(dt);
    this->ui->StartEdit->setText(list[2].toString());
    this->ui->EndEdit->setText(list[3].toString());
    this->ui->CarBox->setValue(list[4].toInt());
    this->ui->SeatBox->setValue(list[5].toInt());
    //设置班次号只能输入字母和数字
    ui->TrainEdit->setAttribute(Qt::WA_InputMethodEnabled, false);
}

void TrainEditDialog::on_okBtn_clicked()//按下OK时
{
    //获取填写的信息
    QString trainclass=ui->TrainEdit->text().trimmed();
    QDateTime StartDT=ui->dtEdit->dateTime();
    QString StartCity=ui->StartEdit->text().trimmed();
    QString EndCity=ui->EndEdit->text().trimmed();
    int CarNum=ui->CarBox->value();
    int SeatNum=ui->SeatBox->value();
    //数据不能为空
    if(trainclass.isEmpty()||StartCity.isEmpty()||EndCity.isEmpty()){
        QMessageBox::critical(this,"错误！","数据不能为空！");
        //清除空格
        ui->TrainEdit->setText(trainclass.trimmed());
        ui->StartEdit->setText(StartCity.trimmed());
        ui->EndEdit->setText(EndCity.trimmed());
        return;
    }
    //数据不能无效
    if(trainclass.length()>10){
        QMessageBox::critical(this,"错误！","列车班次号过长！");
        return;
    }
    //列车班次号码要符合格式
    //开头必为字母，只能为字母（大小写将转化），除了第一个字母外其余均为数字
    QRegExp regexp("^[a-zA-Z]{1}[0-9]+$");//正则表达式
    qDebug()<<"regexp Validation: "<<regexp.isValid();
    qDebug()<<ui->TrainEdit->text().trimmed();
    int match=regexp.indexIn(ui->TrainEdit->text().trimmed());
    qDebug()<<"match result="<<match;
    if(match<0){
        qDebug()<<"Train Class not match";
        QMessageBox::critical(this,"错误！","列车班次格式不匹配！必须为一个字母+数字");
        return;
    }
    //创建新的Train对象并传导给外部的train
    train=new Train(trainclass.trimmed().toUpper(),StartDT,StartCity.trimmed(),EndCity.trimmed(),CarNum,SeatNum);
    qDebug()<<"成功2";
    this->close();
}

void TrainEditDialog::on_cancelBtn_clicked()//按下Cancel时
{
    if(train)delete train;
    train=0;
    close();
}

void TrainEditDialog::SetTitle(QString str){
    this->setWindowTitle(str);
    ui->label_tt->setText(str);
}


