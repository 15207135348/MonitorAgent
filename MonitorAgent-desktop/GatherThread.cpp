#include "GatherThread.h"

#include <QDateTime>
#include <QVariant>


#define NUMBER_POINT2 2

GatherThread::GatherThread(unsigned int internal, QObject *parent):
    QThread(parent)
{
    m_internal = internal;
    m_baseGather = BaseGather::getInstance();
    m_cpuGather = new CpuGather(m_internal/4);
    m_ramGather = new RamGather();
    m_diskGather = new DiskGather(m_internal/4);
    m_netGather = new NetworkGather(m_internal/4);
}
GatherThread::~GatherThread()
{

}
void GatherThread::run()
{
    while (m_run) {
        qint64 t1 = QDateTime::currentMSecsSinceEpoch();
        //汇总信息包
        QMap<QString,QVariant> map;
        //时间戳
        map["time"] = t1;
        map["mac"] = m_baseGather->mac();
        if(m_cpuGather->compute())
        {
            map["cpu"] = m_cpuGather->usage();
            map["user"] = m_cpuGather->user();
            map["system"] = m_cpuGather->system();
            map["iowait"] = m_cpuGather->iowait();
        }
        if(m_ramGather->compute())
        {
            map["ram"] = m_ramGather->usage();
            map["cached"] = m_ramGather->cached();
            map["buffers"] = m_ramGather->buffers();
            map["actual"] = m_ramGather->actual();
        }
        if(m_diskGather->compute())
        {
            map["disk"] = m_diskGather->iokbs();
            map["iops"] = m_diskGather->iops();
        }
        map["net"] = m_netGather->kbs();
        //触发信息槽
        emit gatherDone(map);
        qint64 t2 = QDateTime::currentMSecsSinceEpoch();
        int ms = m_internal - (t2-t1);
        if(ms > 0)
        {
          QThread::msleep(ms);
        }
    }
}
void GatherThread::stop()
{
    m_run = false;
}


