#include "CpuGather.h"
#include <QFile>
#include <QThread>

static long sum(long times[]){
    long all = 0;
    for(int i = 0; i < 7; ++i){
        all+=times[i];
    }
    return all;
}

CpuGather::CpuGather(int internal):
    m_internal(internal)
{

}
/**
 * 获取系统CPU时间
 * 0:user               用户时间
 * 1:nice               用户优先级时间
 * 2:system             系统时间
 * 3:idle               除磁盘IO时间以外的CPU空闲时间
 * 4:iowait             磁盘IO时间
 * 5:irq                硬件中断时间
 * 6:softirq            软件中断时间
*/
bool CpuGather::fill(int i)
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
bool CpuGather::compute()
{
    if(!fill(0)) return false;
    QThread::msleep(m_internal);
    if(!fill(1)) return false;
    double dt = sum(m_times[1]) - sum(m_times[0]);
    if(dt == 0) return false;
    m_usage[0] = (m_times[1][0] - m_times[0][0]) / dt;
    m_usage[1] = (m_times[1][1] - m_times[0][1]) / dt;
    m_usage[2] = (m_times[1][2] - m_times[0][2]) / dt;
    m_usage[3] = (m_times[1][3] - m_times[0][3]) / dt;
    m_usage[4] = (m_times[1][4] - m_times[0][4]) / dt;
    m_usage[5] = (m_times[1][5] - m_times[0][5]) / dt;
    m_usage[6] = (m_times[1][6] - m_times[0][6]) / dt;
    return true;
}

double CpuGather::user()
{
    return m_usage[0];
}
double CpuGather::nice()
{
    return m_usage[1];
}
double CpuGather::system()
{
    return m_usage[2];
}
double CpuGather::idle()
{
    return m_usage[3];
}
double CpuGather::iowait()
{
    return m_usage[4];
}
double CpuGather::irq()
{
    return m_usage[5];
}
double CpuGather::softirq()
{
    return m_usage[6];
}
double CpuGather::usage()
{
    return 1- idle();
}

