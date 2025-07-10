#ifndef USERMAINWINDOW_H
#define USERMAINWINDOW_H

#include <QMainWindow>
#include"datasystem.h"
#include"userbuy.h"
#include"userreturn.h"
#include"changepsw.h"
#include"login.h"
#include"aboutdialog.h"
#include<QCloseEvent>

namespace Ui {
class userMainWindow;
}

class userMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit userMainWindow(DataSystem*,Login*,const QString&,QWidget *parent = 0);
    ~userMainWindow();

private slots:
    void on_buyBtn_clicked();

    void on_returnBtn_clicked();

protected:
    void closeEvent(QCloseEvent* event)override;

private:
    Ui::userMainWindow *ui;
    QString id;
    DataSystem* data;
    Login* login;
};

#endif // USERMAINWINDOW_H
