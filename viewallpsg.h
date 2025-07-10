#ifndef VIEWALLPSG_H
#define VIEWALLPSG_H

#include <QDialog>
#include"datasystem.h"

namespace Ui {
class ViewAllPsg;
}

class ViewAllPsg : public QDialog
{
    Q_OBJECT

public:
    explicit ViewAllPsg(DataSystem* input,const QString&train,QWidget *parent = 0);
    ~ViewAllPsg();

private slots:
    void on_returnBtn_clicked();

private:
    Ui::ViewAllPsg *ui;
    DataSystem* data;
};

#endif // VIEWALLPSG_H
