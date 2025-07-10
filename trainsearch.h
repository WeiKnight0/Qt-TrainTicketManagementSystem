#ifndef TRAINSEARCH_H
#define TRAINSEARCH_H

#include <QDialog>
#include<QTableWidget>
#include<QTableWidgetItem>
#include"datasystem.h"
#include"viewallpsg.h"

namespace Ui {
class TrainSearch;
}

class TrainSearch : public QDialog
{
    Q_OBJECT

public:
    explicit TrainSearch(DataSystem*,QWidget *parent = 0);
    ~TrainSearch();

private slots:

    void on_SearchBtn_clicked();

    void on_ReturnBtn_clicked();

private:
    Ui::TrainSearch *ui;
    DataSystem *data;
};

#endif // TRAINSEARCH_H
