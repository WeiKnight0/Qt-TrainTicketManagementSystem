#ifndef USERREGISTER_H
#define USERREGISTER_H

#include <QDialog>
#include"datasystem.h"

namespace Ui {
class userRegister;
}

class userRegister : public QDialog
{
    Q_OBJECT

public:
    explicit userRegister(DataSystem*, QWidget *parent = 0);
    ~userRegister();

private slots:
    void on_returnBtn_clicked();

    void on_confirmBtn_clicked();

private:
    Ui::userRegister *ui;
    DataSystem* data;
};

#endif // USERREGISTER_H
