#include "userregister.h"
#include "ui_userregister.h"
#include<QMessageBox>

userRegister::userRegister(DataSystem* input,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::userRegister),
    data(input)
{
    ui->setupUi(this);
    setWindowFlags(Qt::WindowCloseButtonHint);
    ui->maleRB->setChecked(true);
    //设置用户名只能是字母和数字
    ui->userEdit->setAttribute(Qt::WA_InputMethodEnabled, false);
    //设置密码输入
    ui->pswEdit->setAttribute(Qt::WA_InputMethodEnabled, false);
    ui->pswEdit_2->setAttribute(Qt::WA_InputMethodEnabled, false);
    //设置证件号只能输入字母和数字
    ui->idEdit->setAttribute(Qt::WA_InputMethodEnabled, false);
    //设置电话号码只能输入数字
    QValidator *validator=new QIntValidator(ui->phoneEdit);
    ui->phoneEdit->setAttribute(Qt::WA_InputMethodEnabled, false);
    ui->phoneEdit->setValidator(validator);
    //日期框
    ui->dateEdit->setDate(QDate::currentDate());
}

userRegister::~userRegister()
{
    delete ui;
}

void userRegister::on_returnBtn_clicked()
{
    this->close();
}

void userRegister::on_confirmBtn_clicked()
{
    QString user=ui->userEdit->text().trimmed();
    QString psw=ui->pswEdit->text().trimmed();
    QString psw_2=ui->pswEdit_2->text().trimmed();
    if(psw!=psw_2){
        QMessageBox::critical(this,"注册失败","两次输入的密码不一致！");
        return;
    }
    if(user.isEmpty()||psw.isEmpty()){
        QMessageBox::critical(this,"提示","用户名或密码均不能为空！");
        return;
    }
    QString name=ui->NameEdit->text().trimmed();
    QString id=ui->idEdit->text().trimmed();
    QString nation=ui->nationEdit->text().trimmed();
    QString phone=ui->phoneEdit->text().trimmed();
    if(name.isEmpty()||id.isEmpty()||nation.isEmpty()||phone.isEmpty()){
        QMessageBox::critical(this,"提示","乘客的姓名/证件号/国籍/电话号码均不能为空！");
        return;
    }
    QSqlQuery*query=data->getNewQuery();
    if(!query->exec(QString("SELECT * FROM users WHERE account='%1'").arg(user))){
        qDebug()<<"错误1！";
        delete query;
        QMessageBox::critical(this,"错误！","系统故障！failed to select users");
        delete this;
        return;
    }
    if(query->next()){
        QMessageBox::information(this,"注册失败！","用户已存在，请重新注册！");
        delete query;
        return;
    }
    if(!query->exec(QString("SELECT * FROM users WHERE relatedID='%1'").arg(id))){
        qDebug()<<"错误2！";
        delete query;
        QMessageBox::critical(this,"错误！","系统故障！failed to select related id");
        delete this;
        return;
    }
    if(query->next()){
        QMessageBox::information(this,"注册失败","乘客已绑定其他用户，请重新注册！");
        delete query;
        return;
    }
    if(!query->exec(QString("SELECT * FROM passengers WHERE id='%1'").arg(id))){
        qDebug()<<"错误3！";
        delete query;
        QMessageBox::critical(this,"错误！","系统故障！failed to select passenger");
        delete this;
        return;
    }
    if(query->next()){//如果已经存在乘客了
        if(!query->exec(QString("INSERT INTO users VALUES('%1','%2','%3')")
                        .arg(user).arg(psw).arg(id))){
            qDebug()<<"错误4！";
            delete query;
            QMessageBox::critical(this,"错误！","系统故障！failed to insert passenger");
            delete this;
            return;
        }
        QMessageBox::information(this,"提示","注册成功！");
        delete query;
        return;
    }
    else{//乘客不存在，需要添加
        //获取性别
        bool gender=true;
        if(ui->maleRB->isChecked()&&!ui->femaleRB->isChecked()){
            gender=true;
        }
        else if(!ui->maleRB->isChecked()&&ui->femaleRB->isChecked()){
            gender=false;
        }
        QString exe=QString("INSERT INTO passengers VALUES('%1',%2,'%3','%4','%5','%6')")
                                .arg(name).arg(gender).arg(ui->dateEdit->date().toString("yyyy-MM-dd"))
                                .arg(id).arg(nation).arg(phone);
        qDebug()<<exe;
        if(!query->exec(exe))
        {
            qDebug()<<"错误5！";
            delete query;
            QMessageBox::critical(this,"错误！","系统故障！failed to select passenger");
            delete this;
            return;
        }
        if(!query->exec(QString("INSERT INTO users VALUES('%1','%2','%3')")
                        .arg(user).arg(psw).arg(id))){
            qDebug()<<"错误6！";
            delete query;
            QMessageBox::critical(this,"错误！","系统故障！failed to select passenger");
            delete this;
            return;
        }
        QMessageBox::information(this,"提示","注册成功！");
        delete query;
        this->close();
        return;
    }

}
