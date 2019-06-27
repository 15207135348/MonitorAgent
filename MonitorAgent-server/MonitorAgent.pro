QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

QT += network
QT += xml

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += main.cpp \
    gather/BaseGather.cpp \
    gather/CpuGather.cpp \
    gather/DiskGather.cpp \
    gather/HistoryGather.cpp \
    gather/NetworkGather.cpp \
    gather/RamGather.cpp \
    gather/UserGather.cpp \
    util/ConfigUtil.cpp \
    util/Fileutil.cpp \
    client.cpp \
    mainconsole.cpp \
    historygthread.cpp \
    metricsgthread.cpp \
    usergthread.cpp \
    action/iptables.cpp \
    gather/PidGather.cpp \
    gather/DevGather.cpp

HEADERS += \
    gather/BaseGather.h \
    gather/CpuGather.h \
    gather/DiskGather.h \
    gather/HistoryGather.h \
    gather/NetworkGather.h \
    gather/RamGather.h \
    gather/UserGather.h \
    util/ConfigUtil.h \
    util/Fileutil.h \
    client.h \
    mainconsole.h \
    historygthread.h \
    metricsgthread.h \
    usergthread.h \
    action/iptables.h \
    gather/PidGather.h \
    gather/DevGather.h
