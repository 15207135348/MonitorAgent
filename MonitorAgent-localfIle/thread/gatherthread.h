#ifndef GATHERTHEAD_H
#define GATHERTHEAD_H

#include <QThread>
#include <gather/BaseGather.h>
#include <gather/CpuGather.h>
#include <gather/RamGather.h>
#include <gather/DiskGather.h>
#include <gather/NetworkGather.h>
#include <gather/PidGather.h>
#include <gather/DevGather.h>
#include <gather/HistoryGather.h>

class GatherThread : public QThread
{
    Q_OBJECT

public:
    GatherThread(QObject *parent=nullptr);
    ~GatherThread();

private:
    //数据采集器
    long long host_file_id = 1;
    long long dev_file_id = 1;
    BaseGather* m_baseGather;
    CpuGather* m_cpuGather;
    RamGather* m_ramGather;
    DiskGather* m_diskGather;
    NetworkGather* m_netGather;
    PidGather* m_pidGather;
    DevGather* m_devGather;
    HistoryGather* m_hisGather;
protected:
    void run();
    void setSelfStart(bool selfstart);
    void log_host(QString& str);
    void log_dev(QString& str);
    void log_pid(QString& str, qint64 t);
    void show();

};
#endif // GATHERTHEAD_H
