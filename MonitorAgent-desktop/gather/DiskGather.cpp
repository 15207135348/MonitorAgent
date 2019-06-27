#include "DiskGather.h"
#include <QFile>
#include <QThread>
#include <gather/BaseGather.h>
//用于获取文件系统的信息
#include <sys/vfs.h>

DiskGather::DiskGather(int internal):
    m_internal(internal),m_sec(m_internal/1000.0)
{

}
/** 获取多个磁盘读写次数和毫秒时间
   8       0      sda     85570       8319        4614862      1011516       64744     62026      5559144     2347540               0                 637736                  3358972
 主设备号 次设备号 设备名称 读完成次数  合并读完成次数   读扇区的次数  读花费的毫秒数   写完成次数 合并写完成次数 写扇区次数 写操作花费的毫秒数 正在处理的输入/输出请求数 输入/输出操作花费的毫秒数 输入/输出操作花费的加权毫秒数
*
* return QMap<QString,QList<long>>
* key:磁盘名;value:磁盘读写次数和毫秒时间
*/
bool DiskGather::fill(int i){

    QFile file("/proc/diskstats");
    if(!file.open(QIODevice::ReadOnly)){
        return false;
    }
    long kb = 0;
    long ct = 0;
    char diskName[16] = "";
    int a,c,d,f;
    long r_count,w_count;
    long r_sectors,w_sectors;
    QList<QString> diskNames = BaseGather::getInstance()->diskNames();
    QString line = file.readLine();
    while(line!=NULL)
    {
        const char* str = line.toLatin1().constData();
        int match = sscanf(str,
                           "   %d       0 %s %ld %d %ld %d %ld %d %ld*",
                           &a,
                           diskName,
                           &r_count,
                           &c,
                           &r_sectors,
                           &d,
                           &w_count,
                           &f,
                           &w_sectors);
        line = file.readLine();
        if(match != 9)
        {
            continue;
        }
        if(!diskNames.contains(diskName))
        {
            continue;
        }
        kb += r_sectors/2;
        kb += w_sectors/2;
        ct += r_count;
        ct += w_count;
    }
    file.close();
    m_value[i][0] = kb;
    m_value[i][1] = ct;
    return true;
}
bool DiskGather::compute()
{
    if(!fill(0)) return false;
    QThread::msleep(m_internal);
    if(!fill(1)) return false;

    m_usage[0] = (double)(m_value[1][0] - m_value[0][0])/m_sec;
    m_usage[1] = (double)(m_value[1][1] - m_value[0][1])/m_sec;

    return true;
}
double DiskGather::iokbs()
{
    return m_usage[0];
}

double DiskGather::iops()
{
    return m_usage[1];
}
//获取剩余磁盘空间
double DiskGather::fsize()
{
    double free = 0;
    QFile file("/proc/mounts");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        return -1;
    }
    struct statfs buf;
    QString line = file.readLine();

    while (line!=NULL) {
        QString mountPoint = line.split(" ").at(1);
        if(statfs(mountPoint.toLatin1(),&buf)){
            line = file.readLine();
            continue;
        }
        free +=double((unsigned long long) buf.f_bfree * buf.f_bsize + 1024 / 2 ) /1024/1024/1024;
        line = file.readLine();
    }
    file.close();
    return free;
}
