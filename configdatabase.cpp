#include "configdatabase.h"
#include "ui_configdatabase.h"
#include <QDebug>
#include<QFileDialog>
#include<QFileInfo>
#include<QDir>
#include<QMessageBox>

ConfigDatabase::ConfigDatabase(DataSystem*input,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigDatabase),
    data(input)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags()&~Qt::WindowContextHelpButtonHint);
    ui->comboBox->setCurrentIndex(0);
    ui->stackedWidget->setCurrentIndex(0);
    //连接下拉框和栈框
    void (QComboBox::*sig)(int)=&QComboBox::currentIndexChanged;
    connect(ui->comboBox, sig,this,[=](int i){
        ui->stackedWidget->setCurrentIndex(i);
    });
}

ConfigDatabase::~ConfigDatabase()
{
    delete ui;
}

void ConfigDatabase::on_confirmBtn_clicked()
{
    int index=ui->comboBox->currentIndex();
    if(index==0){
        //QODBC
        QString ip=ui->ipEdit->text().trimmed();
        QString name=ui->nameEdit->text().trimmed();
        QString acc=ui->accEdit->text().trimmed();
        QString psw=ui->pswEdit->text().trimmed();
        data->setConnection(ip,name,acc,psw);
    }
    if(index==1){
        QString path=ui->pathEdit->text().trimmed();
        if(path.isEmpty()){
            QMessageBox::information(this,"提示","文件路径不得为空");
            return;
        }
        //检测后缀
        QFileInfo fileinfo(path);
        QString fsuffix=fileinfo.suffix();
        qDebug()<<fsuffix;
        if(fsuffix!="db"){
            QMessageBox::critical(this,"错误","请选择数据库文件！");
            return;
        }
        data->setConnection(path);
    }
    if(!data->CheckConnection()){
        QMessageBox::critical(nullptr,"错误！","数据库连接失败！！");
        return;
    }
    QMessageBox::information(this,"提示","数据库连接成功");
    data->createTables();
    this->close();
}

void ConfigDatabase::on_chooseBtn_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "打开数据库文件", "" , "SQLite Database (*.db);;All Files (*)");
    if (!fileName.isEmpty()) {
        // Do something with the selected file
        qDebug() << "Selected file:" << fileName;
        ui->pathEdit->setText(fileName);
    }
}

void ConfigDatabase::on_cancelBtn_clicked()
{
    this->close();
}
