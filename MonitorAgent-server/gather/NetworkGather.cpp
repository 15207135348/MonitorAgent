#include "NetworkGather.h"
#include <QFile>
#include <QThread>


NetworkGather::NetworkGather(int internal):
    m_internal(internal),m_divisor(m_internal*1.024)
{

}
/*获取网络流量信息*/
bool NetworkGather::fill(int i)
{
    QFile file("/proc/net/dev");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        return false;
    }
    long ib=0;
    long ob=0;
    file.readLine();
    file.readLine();
    QString line = file.readLine();
    while (line!=NULL) {
        int pos = line.indexOf(":");
        line = line.mid(pos+1);
        QList<QString> list = line.split(" ");
        QList<QString> list2;
        for(int i=0;i!=list.size();++i)
        {
            if(!list.at(i).isEmpty())
            {
                list2.append(list.at(i));
            }
        }
        if(list2.size()>8)
        {
            ib+=list2.at(0).toLong();
            ob+=list2.at(8).toLong();
        }
        line = file.readLine();
    }
    file.close();
    m_value[i][0] = ib;
    m_value[i][1] = ob;
    return true;
}
bool NetworkGather::compute()
{
    if(!fill(0)) return false;
    QThread::msleep(m_internal);
    if(!fill(1)) return false;
    m_usage[0] = (m_value[1][0] - m_value[0][0]) / m_divisor;
    m_usage[1] = (m_value[1][1] - m_value[0][1]) / m_divisor;
    return true;
}

double NetworkGather::ikbs()
{
    return m_usage[0];
}
double NetworkGather::okbs()
{
    return m_usage[1];
}



