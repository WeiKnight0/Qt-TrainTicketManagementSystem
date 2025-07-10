#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include"datasystem.h"
#include"trainmanage.h"
#include"trainsearch.h"
#include"passengermanage.h"
#include"buytickets.h"
#include"returntickets.h"
#include"changepsw.h"
#include"login.h"
#include"filewindow.h"
#include"aboutdialog.h"
#include<QCloseEvent>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(DataSystem* data_input,const QString&,Login*, QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_TrainBtn_clicked();

    void on_SearchBtn_clicked();

    void on_PassengerBtn_clicked();

    void on_BuyBtn_clicked();

    void on_ReturnBtn_clicked();

    void on_FileBtn_clicked();
protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::MainWindow *ui;
    DataSystem *data;
    QString adminname;
    Login* login;//登录窗口
};

#endif // MAINWINDOW_H
