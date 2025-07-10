#include "psgeditdialog.h"
#include "ui_psgeditdialog.h"
#include<QDebug>
#include<exception>
#include<QMessageBox>
#include<QValidator>//验证器

//没有初始值的构造函数
PsgEditDialog::PsgEditDialog(Passenger*&address,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PsgEditDialog),
    psg(address)
{
    ui->setupUi(this);
    ui->maleRB->setChecked(true);
    setWindowFlags(windowFlags()&~Qt::WindowCloseButtonHint&~Qt::WindowContextHelpButtonHint);
    //日期框显示当前日期
    ui->dateEdit->setDate(QDate::currentDate());
    //设置证件号只能输入字母和数字
    ui->idEdit->setAttribute(Qt::WA_InputMethodEnabled, false);
    //设置电话号码只能输入数字
    QValidator *validator=new QIntValidator(ui->phoneEdit);
    ui->phoneEdit->setAttribute(Qt::WA_InputMethodEnabled, false);
    ui->phoneEdit->setValidator(validator);
}

PsgEditDialog::PsgEditDialog(Passenger *& address,QList<QVariant>& list, QWidget *parent)
    :QDialog(parent),
      ui(new Ui::PsgEditDialog),
      psg(address)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags()&~Qt::WindowCloseButtonHint&~Qt::WindowContextHelpButtonHint);
    this->ui->NameEdit->setText(list[0].toString());
    this->setGender(list[1].toBool());
    this->ui->dateEdit->setDate(list[2].toDate());
    this->ui->idEdit->setText(list[3].toString());
    this->ui->nationEdit->setText(list[4].toString());
    this->ui->phoneEdit->setText(list[5].toString());
    //设置证件号只能输入字母和数字
    ui->idEdit->setAttribute(Qt::WA_InputMethodEnabled, false);
    //设置电话号码只能输入数字
    QValidator *validator=new QIntValidator(ui->phoneEdit);
    ui->phoneEdit->setAttribute(Qt::WA_InputMethodEnabled, false);
    ui->phoneEdit->setValidator(validator);
}

PsgEditDialog::~PsgEditDialog()
{
    delete ui;
}
//获取性别
bool PsgEditDialog::getGender(){
    if(ui->maleRB->isChecked()&&!ui->femaleRB->isChecked()){
        return true;
    }
    else if(!ui->maleRB->isChecked()&&ui->femaleRB->isChecked()){
        return false;
    }
    else{
        throw std::runtime_error("GENDER ERROR!");//错误
    }
}
//设置性别
void PsgEditDialog::setGender(bool value){
    if(value){
        ui->maleRB->setChecked(true);
        ui->femaleRB->setChecked(false);
    }
    else{
        ui->femaleRB->setChecked(true);
        ui->maleRB->setChecked(false);
    }
}

void PsgEditDialog::SetTitle(const QString &str){
    setWindowTitle(str);
    ui->label_tt->setText(str);
}

void PsgEditDialog::on_okBtn_clicked()//按下确认键时
{
    QString name=ui->NameEdit->text().trimmed();
    bool gender;
    try{gender=getGender();}catch(std::runtime_error& exp){qDebug(exp.what());return;}
    QDate birth=ui->dateEdit->date();
    QString id=ui->idEdit->text().trimmed();
    QString nation=ui->nationEdit->text().trimmed();
    QString phone=ui->phoneEdit->text().trimmed();
    if(name.isEmpty()||id.isEmpty()||nation.isEmpty()||phone.isEmpty()){
        QMessageBox::critical(this,"提示","姓名/证件号/国籍/电话号码均不能为空！");
        return;
    }
    //不为空，创建Passenger对象
    psg=new Passenger(name,gender,birth,id,nation,phone);
    this->close();
}

//按下取消键时
void PsgEditDialog::on_cancelBtn_clicked()
{
    if(psg)delete psg;
    psg=0;
    close();
}
