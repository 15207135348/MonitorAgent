#include "metricsgthread.h"

#include <QDateTime>
#include <QVariant>


#define NUMBER_POINT2 2

MetricsGThread::MetricsGThread(unsigned int internal, QObject *parent):
    QThread(parent)
{
    m_internal = internal;
    m_baseGather = BaseGather::getInstance();
    m_cpuGather = new CpuGather(m_internal/4);
    m_ramGather = new RamGather();
    m_diskGather = new DiskGather(m_internal/4);
    m_netGather = new NetworkGather(m_internal/4);
}
MetricsGThread::~MetricsGThread()
{

}
void MetricsGThread::run()
{
    while (m_run) {
        qint64 t1 = QDateTime::currentMSecsSinceEpoch();
        //汇总信息包
        QMap<QString,QVariant> map;
        //时间戳
        map["H"] = "2";
        map["T"] = t1/1000;
        map["M"] = m_baseGather->mac();
        if(m_cpuGather->compute())
        {
            map["a"] = m_cpuGather->user();
            map["b"] = m_cpuGather->nice();
            map["c"] = m_cpuGather->system();
            map["d"] = m_cpuGather->idle();
            map["e"] = m_cpuGather->iowait();
            map["f"] = m_cpuGather->irq();
            map["g"] = m_cpuGather->softirq();
        }
        if(m_ramGather->compute())
        {
            map["h"] = m_ramGather->cached();
            map["i"] = m_ramGather->buffers();
            map["j"] = m_ramGather->actual();
        }
        if(m_diskGather->compute())
        {
            map["k"] = m_diskGather->ikbs();
            map["l"] = m_diskGather->okbs();
            map["m"] = m_diskGather->ips();
            map["n"] = m_diskGather->ops();
        }
        if(m_netGather->compute())
        {
            map["o"] = m_netGather->ikbs();
            map["p"] = m_netGather->okbs();
        }
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
void MetricsGThread::stop()
{
    m_run = false;
}


