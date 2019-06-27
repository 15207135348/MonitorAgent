#-------------------------------------------------
#
# Project created by QtCreator 2018-12-28T21:34:49
#
#-------------------------------------------------

QT       += core gui
QT += network
QT += xml
QT += charts
CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MonitorAgent
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    util/ConfigUtil.cpp \
    util/Fileutil.cpp \
    client.cpp \
    GatherThread.cpp \
    gather/HistoryGather.cpp \
    gather/CpuGather.cpp \
    gather/DiskGather.cpp \
    gather/NetworkGather.cpp \
    gather/RamGather.cpp \
    gather/UserGather.cpp \
    gather/BaseGather.cpp \
    setdialog.cpp

HEADERS += \
        mainwindow.h \
    util/ConfigUtil.h \
    util/Fileutil.h \
    client.h \
    GatherThread.h \
    gather/HistoryGather.h \
    gather/CpuGather.h \
    gather/DiskGather.h \
    gather/NetworkGather.h \
    gather/RamGather.h \
    gather/UserGather.h \
    gather/BaseGather.h \
    setdialog.h

FORMS += \
        mainwindow.ui \
    setdialog.ui

DISTFILES +=

RESOURCES += \
    img.qrc
