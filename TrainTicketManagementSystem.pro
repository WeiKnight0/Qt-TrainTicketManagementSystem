#-------------------------------------------------
#
# Project created by QtCreator 2024-08-24T10:36:19
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TrainTicketManagementSystem
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    login.cpp \
    datasystem.cpp \
    trainmanage.cpp \
    train.cpp \
    traineditdialog.cpp \
    trainsearch.cpp \
    passengermanage.cpp \
    psgeditdialog.cpp \
    passenger.cpp \
    buytickets.cpp \
    returntickets.cpp \
    edittransfer.cpp \
    filewindow.cpp \
    usermainwindow.cpp \
    userregister.cpp \
    userbuy.cpp \
    userreturn.cpp \
    configdatabase.cpp \
    changepsw.cpp \
    viewallpsg.cpp \
    aboutdialog.cpp

HEADERS  += mainwindow.h \
    login.h \
    datasystem.h \
    trainmanage.h \
    train.h \
    traineditdialog.h \
    trainsearch.h \
    passengermanage.h \
    psgeditdialog.h \
    passenger.h \
    buytickets.h \
    returntickets.h \
    edittransfer.h \
    filewindow.h \
    usermainwindow.h \
    userregister.h \
    userbuy.h \
    userreturn.h \
    configdatabase.h \
    changepsw.h \
    viewallpsg.h \
    aboutdialog.h

FORMS    += mainwindow.ui \
    login.ui \
    trainmanage.ui \
    traineditdialog.ui \
    trainsearch.ui \
    passengermanage.ui \
    psgeditdialog.ui \
    buytickets.ui \
    returntickets.ui \
    edittransfer.ui \
    filewindow.ui \
    usermainwindow.ui \
    userregister.ui \
    userbuy.ui \
    userreturn.ui \
    configdatabase.ui \
    changepsw.ui \
    viewallpsg.ui \
    aboutdialog.ui

RESOURCES += \
    resource.qrc

win32 {

RC_ICONS = icon.ico #应用程序图标

}
