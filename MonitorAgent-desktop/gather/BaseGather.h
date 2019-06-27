#ifndef BASEINFOGATHER_H
#define BASEINFOGATHER_H

#include <QObject>

class BaseGather
{

public:

    static BaseGather* getInstance();

    QString machineName();
    QString mac();
    QString ipv4();
    quint16 cpuNumber();
    quint64 totalMemory();
    quint64 totalDisk();
    QList<QString> diskNames();
    QString systemName();

    QJsonObject baseInfo();

private:
    //本机基本信息
    QString m_machineName;
    QString m_macAddress;
    QString m_ipv4Address;
    quint16 m_cpuNumber;
    quint64 m_totalMemory;
    quint64 m_totalDisk;
    QList<QString> m_diskNames;
    QString m_systemName;

    //单例对象
    BaseGather();

protected:
    static BaseGather *_instance;
};

#endif // BASEINFOGATHER_H
