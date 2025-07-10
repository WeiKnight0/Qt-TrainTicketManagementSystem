#include "mainwindow.h"
#include"login.h"
#include"datasystem.h"
#include<QString>
#include <QApplication>
#include<exception>
#include<QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/icon.ico"));//设置图标
    Login loginWindow;
    loginWindow.show();
    return a.exec();
}

