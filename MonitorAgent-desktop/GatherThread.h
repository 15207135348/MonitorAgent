#ifndef GATHERTHEAD_H
#define GATHERTHEAD_H

#include <QThread>

#include <gather/BaseGather.h>
#include <gather/CpuGather.h>
#include <gather/RamGather.h>
#include <gather/DiskGather.h>
#include <gather/NetworkGather.h>

class GatherThread : public QThread
{
    Q_OBJECT

public:
    GatherThread(unsigned int internal, QObject *parent=nullptr);
    ~GatherThread();
    void stop();

private:
    bool m_run = true;
    unsigned int m_internal;
    //数据采集器
    BaseGather* m_baseGather;
    CpuGather* m_cpuGather;
    RamGather* m_ramGather;
    DiskGather* m_diskGather;
    NetworkGather* m_netGather;

signals:
    void gatherDone(const QMap<QString,QVariant>& map);

protected:
    void run();

};
#endif // GATHERTHEAD_H
