#include "RamGather.h"

#include <QFile>
#include <gather/BaseGather.h>

RamGather::RamGather()
{

}

bool RamGather::fill()
{
    QFile file("/proc/meminfo");
    if(!file.open(QIODevice::ReadOnly)){
        return false;
    }
    QString line = file.readLine();
    while (line!=NULL) {
        if(line.contains("MemFree:"))
        {
            m_value[0] = line.replace("MemFree:","").replace(" ","").replace("\n","").replace("kB","").toLong();
        }
        else if(line.contains("MemAvailable:"))
        {
            m_value[1] = line.replace("MemAvailable:","").replace(" ","").replace("\n","").replace("kB","").replace("\t","").toLong();
        }
        else if(line.contains("Buffers:"))
        {
            m_value[2] = line.replace("Buffers:","").replace(" ","").replace("\n","").replace("kB","").replace("\t","").toLong();
        }
        else if(line.contains("Cached:"))
        {
            m_value[3] = line.replace("Cached:","").replace(" ","").replace("\n","").replace("kB","").replace("\t","").toLong();
        }
        line = file.readLine();
    }
    file.close();
    return true;
}

bool RamGather::compute()
{
    if(!fill()) return false;
    qint64 total = BaseGather::getInstance()->totalMemory();
    m_usage[0] = (double)m_value[2] / total;
    m_usage[1] = (double)m_value[3] / total;
    m_usage[2] = (double)(total-m_value[1]) / total;
    m_usage[3] = (double)(total-m_value[0]) / total;
    return true;
}

double RamGather::buffers()
{
    return m_usage[0];
}
double RamGather::cached()
{
    return m_usage[1];
}
double RamGather::actual()
{
    return m_usage[2];
}
double RamGather::usage()
{
    return m_usage[3];
}



