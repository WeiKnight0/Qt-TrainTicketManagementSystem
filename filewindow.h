#ifndef FILEWINDOW_H
#define FILEWINDOW_H

#include <QMainWindow>
#include"datasystem.h"

namespace Ui {
class fileWindow;
}

class fileWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit fileWindow(DataSystem*,QWidget *parent = 0);
    ~fileWindow();

private slots:
    void on_ChoosePathBtn_clicked();

    void on_confirmBtnW_clicked();

    void on_ChooseFileBtn_clicked();

    void on_toReadBtn_clicked();

    void on_toWriteBtn_clicked();

    void on_confirmBtnR_clicked();

    void on_ReturnBtn_clicked();

private:
    Ui::fileWindow *ui;
    DataSystem* data;
};

#endif // FILEWINDOW_H
