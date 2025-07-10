#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include<QPixmap>

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags()&~Qt::WindowContextHelpButtonHint);
    ui->textLabel->setText("版本：1.0.0\n\n开发者：09023110-刘曾渭"
                           "\n\n简介：\n“列车售票管理系统”"
                           "是一款基于 Qt 5.6.1 开源版开发的软件，\n旨在提高列车售票效率"
                           "\n\n本软件使用了 Qt 5.6.1 开源版，更多信息请访问 Qt官方网站。"
                           "\n本软件遵循GPL v3和LGPL v3许可。详细许可证信息请参阅\n"
                           "http://www.qt.io/licensing"
                           " \n\n本软件按“现状”提供，不对其适用性或可用性作任何保证。\n"
                           "使用本软件即表示您接受相关条款和条件。");
    QPixmap pm(":/icon.ico");
    ui->picLable->setPixmap(pm);
    ui->picLable->adjustSize();
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::on_returnBtn_clicked()
{
    this->close();
}
