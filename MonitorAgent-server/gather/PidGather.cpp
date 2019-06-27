#include "PidGather.h"


#include <QDir>
#include <QTextStream>
#include <QThread>
#include <QMap>
#include <fcntl.h>
#include <unistd.h>

//采集进程信息
static const QString PROPATH1 = "/proc/%1/stat";      //cpu
static const QString PROPATH2 = "/proc/%1/status";    //内存
static const QString PROPATH3 = "/proc/%1/io";        //磁盘IO
//PID_STATUS,cpu,disk_read,disk_write
static const QString PID_INFO = "%1,%2,%3,%4\n";
//pid,name,state,threads,mem
static const QString PID_STATUS = "%1,%2,%3,%4,%5";


static long sum(long times[]){
    long all = 0;
    for(int i = 0; i < 7; ++i){
        all+=times[i];
    }
    return all;
}

PidGather::PidGather(int internal, quint64 totalMem)
{
    m_internal = internal;
    m_totalMem = totalMem;
}
/**
 * @brief PidGather::pids
 * @return [pid1,pid2,...]
 */
QList<QString> PidGather::pids()
{
    QList<QString> uIdList;
    QDir dir("/proc");
    if(!dir.exists()){
         return uIdList;
    }
    dir.setFilter(QDir::Dirs);
    QStringList list = dir.entryList();
    for(QString& c : list){
        if(c.toUInt()>0)
            uIdList.append(c);
    }
    return uIdList;
}
/**
 * @brief PidGather::pidState
 * @param pid
 * @return [cpu,disk_read,disk_write]
 */
QList<long> PidGather::pidState(const QString& pid)
{
    /*
     * 获取进程的CPU时间
     * 进程占用CPU时间 = utime + stime + cutime + cstime
     * utime  该任务在用户态运行的时间，单位为jiffies
     * stime  该任务在核心态运行的时间，单位为jiffies
     * cutime 所有已死线程在用户态运行的时间，单位为jiffies
     * cstime 所有已死在核心态运行的时间，单位为jiffies
     */
    QList<long> state;
    QFile file(PROPATH1.arg(pid));
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QList<QByteArray> list = file.readAll().split(' ');
        if(list.size()>16)
        {
            state.append((list.at(13).toLong()+list.at(14).toLong()+list.at(15).toLong()+list.at(16).toLong()));
        }
        file.close();
    }
    //获取进程的读写磁盘字节数量
    int fd = open(PROPATH3.arg(pid).toStdString().c_str(),O_RDONLY);
    if(fd < 0)
    {
        printf("%s open failed\n",PROPATH3.arg(pid).toStdString().c_str());
    }
    else
    {
        char buffer[1024];
        read(fd,buffer,sizeof(buffer));
        QString data = buffer;
        data = data.mid(data.indexOf("read_bytes"));
        QList<QString> list = data.split("\n");
        for(QString& str : list)
        {
            int pos = str.indexOf(":");
            QString key = str.left(pos);
            QString value = str.mid(pos+1);
            if(key=="read_bytes")
            {
                state.append(value.toLong());
            }
            else if(key=="write_bytes")
            {
                state.append(value.toLong());
            }
        }
        close(fd);
    }
    return state;
}
/**
 * @brief PidGather::pidState
 * @param pids
 * @return <pid,[cpu,disk_read,disk_write]>
 */
QMap<QString,QList<long>> PidGather::pidState(const QList<QString>& pids){
    QMap<QString,QList<long>> map;
    for(const QString& pid: pids)
    {
       QList<long> state = pidState(pid);
       if(state.size() == 3){
           map[pid] = state;
       }
    }
    return map;
}
/**
 * @brief PidGather::pidStatus
 * @param pid
 * @return pid,name,state,threads,mem
 */
QString PidGather::pidStatus(const QString& pid){

    QString status;
    QFile file(PROPATH2.arg(pid));
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);
        QString data = in.readAll();
        QList<QString> list = data.split("\n");
        if(list.size()<7)
        {
            return "";
        }
        QString name = list.at(0);
        name = name.replace("Name:","").replace(" ","").replace("\t","");
        QString state = list.at(2);
        state = state.replace("State:","").replace(" ","").replace("\t","");
        state = state.mid(state.indexOf("(")+1);
        state = state.left(state.lastIndexOf(")"));
        QString mem="0";
        QString thread="0";
        for(int i=7;i!=list.size();++i)
        {
            if(list.at(i).contains("VmRSS"))
            {
                mem = list.at(i);
                mem = mem.replace("VmRSS:","").replace(" ","").replace("\t","").replace("kB","");
                mem = QString::number(mem.toDouble() / m_totalMem,'f', 4);
            }
            if(list.at(i).contains("Threads"))
            {
                thread = list.at(i);
                thread = thread.replace("Threads:","").replace(" ","").replace("\t","");
            }
        }
        status = PID_STATUS.arg(pid,name,state,thread,mem);
        file.close();
    }
    return status;
}


QString PidGather::pidInfo(const QList<QString>& pids,
                           const QMap<QString,QList<long>>& pidState1,
                           const QMap<QString,QList<long>>& pidState2,
                           double dt)
{
    QString info;
    for(const QString& pid : pids)
    {
        QString status = pidStatus(pid);
        if(!status.isEmpty())
        {
            double cpuRate=0;
            int diskRead=0,diskWrite=0;
            QList<long> state1 = pidState1[pid];
            QList<long> state2 = pidState2[pid];
            if(state1.size() == 3 && state2.size() == 3)
            {
                 cpuRate = (double)(state2.at(0)-state1.at(0))/dt;
                 diskRead = (state2.at(1)-state1.at(1))/m_internal;//kb/s
                 diskWrite = (state2.at(2)-state1.at(2))/m_internal;//kb/s
            }
            QString line = PID_INFO.arg(status,
                         QString::number(cpuRate,'f',4),
                         QString::number(diskRead),
                         QString::number(diskWrite));
            info.append(line);
        }
    }
    return info;
}

bool PidGather::fill(int i)
{
    QFile file("/proc/stat");
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text)){
        return false;
    }
    QString line = file.readLine();
    const char* str = line.toLatin1().constData();
    file.close();
    sscanf(str, "cpu  %ld %ld %ld %ld %ld %ld %ld *",
           &m_times[i][0],
           &m_times[i][1],
           &m_times[i][2],
           &m_times[i][3],
           &m_times[i][4],
           &m_times[i][5],
           &m_times[i][6]);
    return true;
}

QString PidGather::pidInfo()
{
    QList<QString> list = pids();
    if(!fill(0)) return "";
    QMap<QString,QList<long>> state1 = pidState(list);
    QThread::msleep(m_internal);
    if(!fill(1)) return "";
    double dt = sum(m_times[1]) - sum(m_times[0]);
    QMap<QString,QList<long>> state2 = pidState(list);
    return pidInfo(list,state1,state2,dt);
}
