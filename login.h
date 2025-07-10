#ifndef LOGIN_H
#define LOGIN_H

#include <QMainWindow>
#include<QString>
#include<QShowEvent>
#include"datasystem.h"
#include"aboutdialog.h"

namespace Ui {
class Login;
}

class Login : public QMainWindow
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = 0);
    ~Login();
    //更改数据库显示
    void UpdateDBShow();

protected:
    void showEvent(QShowEvent *event)override{
        event->accept();
        UpdateDBShow();
    }

private slots:
    void on_exitBtn_clicked();

    void on_exitBtn_4_clicked();

    void on_loginBtn_4_clicked();

    void on_loginBtn_clicked();

    void on_registerBtn_clicked();

private:
    Ui::Login *ui;
    DataSystem*data;
};

#endif // LOGIN_H
