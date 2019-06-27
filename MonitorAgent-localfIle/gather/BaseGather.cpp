#include "BaseGather.h"
#include <QHostInfo>
#include <QNetworkInterface>
#include <QFile>
#include <QTextStream>
#include <QJsonObject>
#include <QJsonDocument>

//用于获取文件系统的信息
#include <sys/vfs.h>

/*-----------------------------获取本机基本信息--------------------------*/
static QString getMachineName()
{
    return QHostInfo::localHostName();
}
static QString getIpv4Address ()
{
    QString ipv4;
    QList<QHostAddress> AddressList = QNetworkInterface::allAddresses();
    foreach(QHostAddress address, AddressList){
        if(address.protocol() == QAbstractSocket::IPv4Protocol &&
                address != QHostAddress::Null &&
                address != QHostAddress::LocalHost){
            if (address.toString().contains("127.0.")){
                continue;
            }
            ipv4 = address.toString();
            break;
        }
    }
    return ipv4;
}
static QString getMacAddress()
{
    QList<QNetworkInterface> nets = QNetworkInterface::allInterfaces();// 获取所有网络接口列表
    int nCnt = nets.count();
    QString mac = "";
    for(int i = 0; i < nCnt; i ++)
    {
        // 如果此网络接口被激活并且正在运行并且不是回环地址，则就是我们需要找的Mac地址
        if(nets[i].flags().testFlag(QNetworkInterface::IsUp) &&
           nets[i].flags().testFlag(QNetworkInterface::IsRunning) &&
           !nets[i].flags().testFlag(QNetworkInterface::IsLoopBack))
        {
            mac = nets[i].hardwareAddress();
            break;
        }
    }
    return mac;
}
static quint16 getCpuNum()
{
    quint16 cpu=0;
    QFile file("/proc/stat");
    if(!file.open(QIODevice::ReadOnly)){
        return cpu;
    }
    QString line = file.readLine();
    while (line!=NULL) {
        line = file.readLine();
        if(line.contains("cpu")){
            ++cpu;
        }else{
            break;
        }
    }
    file.close();
    return cpu;
}
static quint64 getTotalMemery()
{
    ulong memery = 0;
    QFile file("/proc/meminfo");
    if(!file.open(QIODevice::ReadOnly))
    {
        return memery;
    }
    QTextStream in(&file);
    QString line = in.readLine();
    while (line!=NULL){
        if(line.contains("MemTotal")){
            memery = line.replace("MemTotal:","").replace(" ","").
                    replace("kB",""). replace("\n","").toLong();
            break;
        }
        line = in.readLine();
    }
    file.close();
    return memery;
}
static quint64 getTotalDisk()
{
    quint64 disk = 0;
    QFile file("/proc/mounts");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        return disk;
    }
    struct statfs buf;
    QTextStream in(&file);
    QString line = in.readLine();
    while (line!=NULL) {
        QString mountPoint = line.split(" ").at(1);
        if(statfs(mountPoint.toLatin1(),&buf)){
            line = in.readLine();
            continue;
        }
        /*block to kbyte*/
        disk +=((unsigned long long) buf.f_blocks * buf.f_bsize + 1024 / 2)/1024;
        line = in.readLine();
    }
    file.close();
    return disk;
}
static QList<QString> getDiskNames(){
    QList<QString> names;
    QFile file("/proc/diskstats");
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        return names;
    }
    QString line = file.readLine();
    while(line!=NULL)
    {
        if(line.at(11) == '0' && !line.contains("loop") && !line.contains("ram"))
        {
            //磁盘名字
            QString name = line.mid(13,3);
            names.append(name);
        }
        line = file.readLine();
    }
    file.close();
    return names;
}
static QString getSystemName()
{
    return QString("%1%2 %3位").
            arg(QSysInfo::productType(),
                QSysInfo::productVersion(),
                QString::number(QSysInfo::WordSize));
}



//初始化单例对象指针
BaseGather* BaseGather::_instance = NULL;

BaseGather* BaseGather::getInstance()
{
    if(_instance == NULL)
    {
        _instance = new BaseGather();
    }
    return _instance;
}
BaseGather::BaseGather()
{
    m_machineName = getMachineName();
    m_macAddress = getMacAddress();
    m_ipv4Address = getIpv4Address();
    m_cpuNumber = getCpuNum();
    m_totalMemory = getTotalMemery();
    m_totalDisk = getTotalDisk();
    m_diskNames = getDiskNames();
    m_systemName = getSystemName();

}
QString BaseGather::machineName(){
    return m_machineName;
}
QString BaseGather::mac(){
    return m_macAddress;
}
QString BaseGather::ipv4(){
    return m_ipv4Address;
}
quint16 BaseGather::cpuNumber(){
    return m_cpuNumber;
}
quint64 BaseGather::totalMemory(){
    return m_totalMemory;
}
quint64 BaseGather::totalDisk(){
    return m_totalDisk;
}
QList<QString> BaseGather::diskNames()
{
    return m_diskNames;
}
QString BaseGather::systemName()
{
    return m_systemName;
}

QJsonObject BaseGather::baseInfo()
{
    QJsonObject hostInfo;
    //操作系统
    hostInfo.insert("SN",systemName());
    //MAC地址
    hostInfo.insert("M",mac());
    //IPV4地址
    hostInfo.insert("IP",ipv4());
    //机器名
    hostInfo.insert("MN",machineName());
    //总内存
    hostInfo.insert("MT",QString::number((double)totalMemory()/1024/1024,'f',1)+"G");
    //总挂载磁盘
    hostInfo.insert("DT",QString::number((double)totalDisk()/1024/1024,'f',1)+"G");
    //总CPU数量
    hostInfo.insert("CN",cpuNumber());

    return hostInfo;
}
