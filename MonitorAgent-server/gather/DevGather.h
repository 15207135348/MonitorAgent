#ifndef DEVGATHER_H
#define DEVGATHER_H

#include <QString>
#include <QStringList>

class DevGather
{
private:
    //CPU温度文件路径 风扇转速文件路径 GPU负载文件路径是否初始化成功
    bool m_init_success;
    //CPU温度文件路径
    QStringList m_cputemp_labels;
    QStringList m_cputemp_alarms;
    QStringList m_cputemp_inputs;
    QStringList m_cputemp_maxs;
    QStringList m_cputemp_crits;
    //风扇转速文件路径
    QStringList m_fanspeed_labels;
    QStringList m_fanspeed_inputs;
    //GPU负载文件路径
    QStringList m_gpuload_labels;
    QStringList m_gpuload_mins;
    QStringList m_gpuload_inputs;
    QStringList m_gpuload_maxs;
public:
    DevGather();
    bool initHwmonNames();
    QString getHwmonInfo();
};

#endif // DEVGATHER_H
