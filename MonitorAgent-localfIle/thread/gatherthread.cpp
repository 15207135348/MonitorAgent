#include "gatherthread.h"
#include <QDateTime>
#include <QVariant>
#include <QFile>
#include <QDir>
#include <QCoreApplication>

#define NUMBER_POINT2 2
#define SELF_RESTART true
#define GATHER_INTERNAL 2000
#define LOD_DIR_DEV "/var/monitor-agent/log/dev"
#define LOD_DIR_HOST "/var/monitor-agent/log/host"
#define LOD_DIR_PID "/var/monitor-agent/log/pid"
#define LOD_DIR_CONSOLE "/var/monitor-agent/log/console"

#define HOST_HEAD "time,user%,nice%,system%,idle%,iowait%,irq%,softirq%,cached%,buffers%,actual%,diskread(kbs),diskwrite(kbs),diskips,diskops,traffic_in(kbs),traffic_out(kbs)\n"
#define DEV_HEAD "time,min_cpu_temp(C),max_cpu_temp(C),min_gpu_load(V),max_gpu_load(V),min_fan(min),max_fan(min)\n"

//128MB
#define MAX_FILE_SIZE 134217728

GatherThread::GatherThread(QObject *parent):
    QThread(parent)
{
    QDir dir;
    dir.mkpath(LOD_DIR_DEV);
    dir.mkpath(LOD_DIR_HOST);
    dir.mkpath(LOD_DIR_PID);
    dir.mkpath(LOD_DIR_CONSOLE);
    setSelfStart(SELF_RESTART);

    m_baseGather = BaseGather::getInstance();
    m_cpuGather = new CpuGather(GATHER_INTERNAL/5);
    m_ramGather = new RamGather();
    m_diskGather = new DiskGather(GATHER_INTERNAL/5);
    m_netGather = new NetworkGather(GATHER_INTERNAL/5);
    m_pidGather = new PidGather(GATHER_INTERNAL/5, m_baseGather->totalMemory());
    m_devGather = new DevGather();
    m_hisGather = HistoryGather::getInstance();

    show();
}
GatherThread::~GatherThread()
{

}
void GatherThread::run()
{
    while (true) {
        qint64 t1 = QDateTime::currentMSecsSinceEpoch();
        if(m_cpuGather->compute()&&
           m_ramGather->compute()&&
           m_diskGather->compute()&&
           m_netGather->compute())
        {
            QString host = QString::number(t1).append(",");
            host.append(QString::number(m_cpuGather->user()*100,'f',NUMBER_POINT2)).append(",");
            host.append(QString::number(m_cpuGather->nice()*100,'f',NUMBER_POINT2)).append(",");
            host.append(QString::number(m_cpuGather->system()*100,'f',NUMBER_POINT2)).append(",");
            host.append(QString::number(m_cpuGather->idle()*100,'f',NUMBER_POINT2)).append(",");
            host.append(QString::number(m_cpuGather->iowait()*100,'f',NUMBER_POINT2)).append(",");
            host.append(QString::number(m_cpuGather->irq()*100,'f',NUMBER_POINT2)).append(",");
            host.append(QString::number(m_cpuGather->softirq()*100,'f',NUMBER_POINT2)).append(",");
            host.append(QString::number(m_ramGather->cached()*100,'f',NUMBER_POINT2)).append(",");
            host.append(QString::number(m_ramGather->buffers()*100,'f',NUMBER_POINT2)).append(",");
            host.append(QString::number(m_ramGather->actual()*100,'f',NUMBER_POINT2)).append(",");
            host.append(QString::number(m_diskGather->ikbs(),'f',NUMBER_POINT2)).append(",");
            host.append(QString::number(m_diskGather->okbs(),'f',NUMBER_POINT2)).append(",");
            host.append(QString::number(m_diskGather->ips(),'f',NUMBER_POINT2)).append(",");
            host.append(QString::number(m_diskGather->ops(),'f',NUMBER_POINT2)).append(",");
            host.append(QString::number(m_netGather->ikbs(),'f',NUMBER_POINT2)).append(",");
            host.append(QString::number(m_netGather->okbs(),'f',NUMBER_POINT2)).append("\n");
            log_host(host);
        }
        if(m_devGather->initHwmonNames())
        {
            QString dev = QString::number(t1).append(",");
            dev.append(m_devGather->getHwmonInfo()).append("\n");
            log_dev(dev);
        }
        QString pid = m_pidGather->pidInfo();
        log_pid(pid,t1);

        qint64 t2 = QDateTime::currentMSecsSinceEpoch();
        int ms = GATHER_INTERNAL - (t2-t1);
        if(ms > 0)
        {
          QThread::msleep(ms);
        }
    }
}

void GatherThread::log_host(QString &str)
{
    QString filename = QString(LOD_DIR_HOST).append("/status.log");
    QFile file(filename);
    if(file.size() > MAX_FILE_SIZE)
    {
        bool res = file.rename(QString(LOD_DIR_HOST).append("/status.log.").append(QString::number(host_file_id++)));
        while (!res)
        {
            res = file.rename(QString(LOD_DIR_HOST).append("/status.log.").append(QString::number(host_file_id++)));
        }
    }
    QFile file1(filename);
    //first create
    if(!file1.exists())
    {
        if(file1.open(QIODevice::Append))
        {
            file1.write(HOST_HEAD);
            file1.write(str.toLatin1());
            file1.close();
        }
    }
    else
    {
        if(file1.open(QIODevice::Append))
        {
            file1.write(str.toLatin1());
            file1.close();
        }
    }
}
void GatherThread::log_dev(QString& str)
{
    QString filename = QString(LOD_DIR_DEV).append("/status.log");
    QFile file(filename);
    if(file.size() > MAX_FILE_SIZE)
    {
        bool res = file.rename(QString(LOD_DIR_DEV).append("/status.log.").append(QString::number(dev_file_id++)));
        while (!res)
        {
            res = file.rename(QString(LOD_DIR_DEV).append("/status.log.").append(QString::number(dev_file_id++)));
        }
    }
    QFile file1(filename);
    //first create
    if(!file1.exists())
    {
        if(file1.open(QIODevice::Append))
        {
            file1.write(DEV_HEAD);
            file1.write(str.toLatin1());
            file1.close();
        }
    }
    else
    {
        if(file1.open(QIODevice::Append))
        {
            file1.write(str.toLatin1());
            file1.close();
        }
    }
}
void GatherThread::log_pid(QString& str, qint64 t)
{
    QString filename = QString(LOD_DIR_PID).append("/status.log.").append(QString::number(t));
    QFile file(filename);
    if(file.open(QIODevice::WriteOnly)){
        file.write(str.toLatin1());
        file.close();
    }
}
void GatherThread::setSelfStart(bool selfstart)
{
    QFile file("/etc/rc.local");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }
    QString data = file.readAll();
    file.close();
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return;
    }
    QList<QString> list = data.split("\n");
    if(selfstart && !list.contains("#set monitor agent self-start"))
    {
        int pos = list.lastIndexOf("exit 0");
        list.insert(pos, "#set monitor agent self-start");
        list.insert(pos+1, QString("nohup %1 >/dev/null 2>&1 &").arg(QCoreApplication::applicationFilePath()));
    }
    else if(!selfstart && list.contains("#set monitor agent self-start"))
    {
        int pos = list.indexOf("#set monitor agent self-start");
        list.removeAt(pos);
        list.removeAt(pos);
    }
    for(QString s : list)
    {
        if(s.isEmpty()) continue;
        file.write(s.append("\n").toLatin1());
    }
    file.close();
}
void GatherThread::show()
{
    printf("gather.internal=%dms\n",GATHER_INTERNAL);
    printf("gather.logdir=/var/monitor-agent/log\n");
    printf("agent.self.start=true\n");
    printf("agent.status=running\n");
}


