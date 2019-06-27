#include "NetworkGather.h"
#include <QFile>
#include <QThread>


NetworkGather::NetworkGather(int internal):
    m_internal(internal),m_divisor(m_internal*1.024)
{

}
/*获取网络流量信息*/
long NetworkGather::traffic(){
    QFile file("/proc/net/dev");

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        return 0;
    }
    long byte=0;
    file.readLine();
    file.readLine();
    QString line = file.readLine();
    while (line!=NULL) {
        line = line.replace(QRegExp("\\s+"),",");
        QStringList split = line.split(",");
        QString rb = split.at(1);
        QString wb = split.at(9);
        byte = byte + rb.toLong() + wb.toLong();
        line = file.readLine();
    }
    file.close();
    return byte;
}
double NetworkGather::kbs()
{
    double byte1 = traffic();
    QThread::msleep(m_internal);
    double byte2 = traffic();
    return (byte2-byte1)/m_divisor;
}



