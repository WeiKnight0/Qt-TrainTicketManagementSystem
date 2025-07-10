#ifndef CHANGEPSW_H
#define CHANGEPSW_H

#include <QDialog>
#include<QStringList>
#include"datasystem.h"

namespace Ui {
class ChangePsw;
}

class ChangePsw : public QDialog
{
    Q_OBJECT

public:
    explicit ChangePsw(QStringList*&,const QString&,QWidget *parent = 0);
    ~ChangePsw();

private slots:
    void on_cancelBtn_clicked();

    void on_confirmBtn_clicked();

private:
    Ui::ChangePsw *ui;
    QStringList*& storage;//储存密码
};

#endif // CHANGEPSW_H
