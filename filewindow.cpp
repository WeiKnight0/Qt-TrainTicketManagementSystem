#include "filewindow.h"
#include "ui_filewindow.h"
#include<QFileDialog>
#include<QDebug>
#include<QDir>
#include<QMessageBox>

fileWindow::fileWindow(DataSystem*input,QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::fileWindow),
    data(input)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
    setWindowFlags(windowFlags()&~Qt::WindowMinMaxButtonsHint);
}

fileWindow::~fileWindow()
{
    delete ui;
}

void fileWindow::on_ChoosePathBtn_clicked()
{
    //选择文件输出路径
    QString filePath = QFileDialog::getSaveFileName(this, "Save Database File", "", "Database Files (*.db);;All Files (*)");
    if (filePath.isEmpty()) {
        qDebug() << "No file selected.";
        return;
    }
    ui->WEdit->setText(filePath);

}

void fileWindow::on_confirmBtnW_clicked()
{
    QString filePath=ui->WEdit->text().trimmed();
    if(filePath.isEmpty()){
        QMessageBox::information(this,"提示","文件路径不得为空！");
        return;
    }
    //检测后缀
    QFileInfo fileinfo(filePath);
    QString fsuffix=fileinfo.suffix();
    if(fsuffix!="db"){
        QMessageBox::critical(this,"错误","输出文件必为数据库文件(.db)！");
        return;
    }
    //判断原数据是mysql还是sqlite
    QString sqlName=data->getDriverName();
    if(sqlName=="QODBC"){
        //ODBC驱动的MySQL
        qDebug()<<"mysql";
        //创建新的SQLite数据库并连接到文件,采用DataSystem类
        DataSystem sqliteSys(filePath,false);//不创建默认管理员
        if(!sqliteSys.CheckConnection()||!this->data->CheckConnection()){
            QMessageBox::critical(this,"错误！","数据库连接失败！！");
            qDebug()<<"失败";
            return;
        }
        QSqlQuery* mysqlQuery=data->getNewQuery();
        QSqlQuery* sqliteQuery=sqliteSys.getNewQuery();
        // 获取 MySQL 数据库中的表名
        QStringList tables = data->getAllTables();
        qDebug()<<"表格："<<tables;
        for (const QString &table : tables) {
            //拷贝表
            qDebug()<<table<<"正在拷贝";
            if(table=="trainclass"||table=="admin"||table=="passengers"||table=="users"){
                //不用创造表
                qDebug()<<"no need to create table";
            }
            else if(table.contains("transfer_")){
                //是中间站表
                qDebug()<<"transfer"<<table;
                QString createSql=QString("CREATE TABLE IF NOT EXISTS %1(indexNum int, City char(22))").
                        arg(table.toLower());
                if (!sqliteQuery->exec(createSql)) {
                    qDebug() << "Error inserting data into SQLite:" << sqliteQuery->lastError().text();
                    return;
                }
                //插入数据
            }
            else{
                //是列车表
                qDebug()<<"train"<<table;
                QString exe=QString("CREATE TABLE IF NOT EXISTS %1(").arg(table.toLower());
                exe.append("Psgid char(22) NOT NULL, Cars int NOT NULL, Seats int NOT NULL,"
                           " FromCity int NOT NULL, ToCity int NOT NULL)");
                if (!sqliteQuery->exec(exe)) {
                    qDebug() << "Error inserting data into SQLite:" << sqliteQuery->lastError().text();
                    return;
                }
            }
            //查询数据
            mysqlQuery->exec(QString("SELECT * FROM %1").arg(table));
            int fieldCount = mysqlQuery->record().count();
            // 插入数据到 SQLite 数据库中
            while (mysqlQuery->next()) {
                QList<QVariant> values;
                for (int i = 0; i < fieldCount; ++i) {
                    values << mysqlQuery->value(i);
                }
                QString insertSql = QString("INSERT INTO %1 VALUES(:var0")
                        .arg(table);
                for(int j=1;j<fieldCount;++j){
                    insertSql.append(QString(", :var%1").arg(j));
                }
                insertSql.append(")");
                sqliteQuery->prepare(insertSql);
                for(int k=0;k<fieldCount;k++){
                    sqliteQuery->bindValue(QString(":var%1").arg(k),values.at(k));
                    qDebug()<<"绑定了"<<QString(":var%1").arg(k)<<sqliteQuery->boundValue(QString(":var%1").arg(k));
                }
                if (!sqliteQuery->exec()) {
                    qDebug()<<"error statement: "<<insertSql;
                    qDebug() << "Error inserting data into SQLite:" << sqliteQuery->lastError().text();
                    return;
                }

            }
        }
        QMessageBox::information(this,"提示","输出文件成功！");
        delete mysqlQuery;
        delete sqliteQuery;
        //关闭文件数据库连接
        sqliteSys.closeConnection();
    }
    else if(sqlName=="QSQLITE"){
        //SQLite
        qDebug()<<"sqlite";
        // 获取原数据库的文件路径
        QString originalFilePath = data->getDBInfo().at(0);
        qDebug()<<"源文件路径"<<originalFilePath;
        // 确保目标文件路径不是原文件路径（避免覆盖）
        if (originalFilePath == filePath) {
            qDebug() << "Target file path is the same as the original file path.";
            return;
        }
        //断开源文件
        QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
        QSqlDatabase::database(QLatin1String(QSqlDatabase::defaultConnection),false).close();
        // 复制数据库文件
        if (QFile::copy(originalFilePath,filePath)) {
            qDebug() << "Database copied successfully to" << filePath;
        } else {
            qDebug() << "Error copying database:";
        }
        //重新连接
        data->setConnection(originalFilePath);
        if(!data->CheckConnection()){
            QMessageBox::critical(this,"错误！","系统故障 unable to reconnect database");
            delete this;
            return;
        }
        QMessageBox::information(this,"提示","输出文件成功！");
    }
    else{
        qDebug()<<"unkown driver name!"<<sqlName;
        delete this;
        return;
    }
}

void fileWindow::on_ChooseFileBtn_clicked()
{
    QString filepath=QFileDialog::getOpenFileName(this,"打开数据库文件","","Database Files (*.db);;All Files (*)");
    ui->REdit->setText(filepath);
}

void fileWindow::on_toReadBtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void fileWindow::on_toWriteBtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void fileWindow::on_confirmBtnR_clicked()
{
    //获取文件
    QString filepath=ui->REdit->text();
    if(filepath.trimmed().isEmpty()){
        QMessageBox::information(this,"提示","文件路径不得为空！");
        return;
    }
    QFile file(filepath);
    if(!file.exists()){
        QMessageBox::information(this,"提示","文件不存在！");
        return;
    }
    //检测后缀
    QFileInfo fileinfo(filepath);
    QString fsuffix=fileinfo.suffix();
    if(fsuffix!="db"){
        QMessageBox::critical(this,"错误","请选择数据库文件！");
        return;
    }
    //弹出警告
    QMessageBox::StandardButton reply=QMessageBox::warning(this,"提示","如果继续，现有的数据库将断开连接，请确保已保存有关数据！\n是否继续？"
                         ,QMessageBox::Yes|QMessageBox::No,QMessageBox::No);
    if(reply==QMessageBox::No){
        return;
    }
    //获取原数据库信息
    QStringList originallist=data->getDBInfo();
    //断开数据库
    QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
    QSqlDatabase::database(QLatin1String(QSqlDatabase::defaultConnection),false).close();
    //连接到新数据库
    data->setConnection(filepath);
    if(!data->CheckConnection()){
        delete this;
        QMessageBox::critical(this,"错误","读取失败！");
        data->setConnection(originallist);
        return;
    }
    data->createTables();
    QMessageBox::information(this,"提示","读取成功！");
}

void fileWindow::on_ReturnBtn_clicked()
{
    this->close();
}
