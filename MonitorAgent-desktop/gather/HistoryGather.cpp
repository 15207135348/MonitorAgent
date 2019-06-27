#include "HistoryGather.h"

#include <QString>
#include <QFile>
#include <QDir>

#define FILE_PATH QString("/tmp/history/%1")

HistoryGather* HistoryGather::_instance = NULL;

HistoryGather* HistoryGather::getInstance()
{
    if(_instance == NULL)
    {
        _instance = new HistoryGather();
    }
    return _instance;
}

HistoryGather::HistoryGather()
{
    init();
}
void HistoryGather::init()
{
    QFile file("/etc/profile");
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        return;
    }
    QString line = file.readLine();
    while(line != NULL)
    {
        if(line=="#Set history log\n")
        {
            file.close();
            return;
        }
        line = file.readLine();
    }
    file.close();
    if(!file.open(QIODevice::Append))
    {
        return;
    }
    QByteArray text =
    "#Set history log\n"
    "USER_IP=`who -u am i 2>/dev/null| awk '{print $NF}'|sed -e 's/[()]//g'`\n"
    "if [ \"$USER_IP\" = \"\" ]\n"
    "then\n"
    "        USER_IP=`hostname`\n"
    "fi\n"
    "if [ ! -d /tmp/history ]\n"
    "then\n"
    "        mkdir /tmp/history\n"
    "        chmod 777 /tmp/history\n"
    "fi\n"
    "export HISTFILE=\"/tmp/history/${LOGNAME}@${USER_IP}\"\n"
    "export HISTFILESIZE=512\n"
    "export HISTTIMEFORMAT=\"[%F %T][`whoami`][${USER_IP}] \n"
    "chmod 600 /tmp/history/*@* 2>/dev/null\n";
    file.write(text);
    file.close();
}
QStringList HistoryGather::ls()
{
    QDir dir("/tmp/history");
    QStringList filters;
    filters << QString("*@*");
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    dir.setNameFilters(filters);
    QStringList list = dir.entryList();
    for(const QString& e : list)
    {
        if(m_fileMap.contains(e))
        {
            m_fileMap[e] = true;
        }
        else
        {
            m_fileMap[e] = false;
        }
    }
    QStringList nfs;
    for(auto it = m_fileMap.constKeyValueBegin(); it!=m_fileMap.constKeyValueEnd(); ++it)
    {
        if((*it).second) continue;
        nfs.append((*it).first);
    }
    return nfs;
}
QList<HistRecord> HistoryGather::history(const QString &filename)
{
    QList<HistRecord> list;
    QFile file(FILE_PATH.arg(filename));
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        return list;
    }
    QString time = file.readLine();
    QString command = file.readLine();
    while(time != NULL)
    {
        HistRecord record;
        record.timestamp = time.mid(1).toLong();
        record.command = command;
        time = file.readLine();
        command = file.readLine();
        list.append(record);
    }
    file.close();
    return list;
}
QList<HistRecord> HistoryGather::history(const QString& filename, long t1, long t2)
{
    QList<HistRecord> list;
    QFile file(FILE_PATH.arg(filename));
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        return list;
    }
    QString time = file.readLine();
    QString command = file.readLine();
    while(time != NULL)
    {
        long ct = time.mid(1).toLong();
        if(ct < t1)
        {
            continue;
        }
        else if(ct > t2)
        {
            break;
        }
        else
        {
            HistRecord record;
            record.timestamp = ct;
            record.command = command;
            time = file.readLine();
            command = file.readLine();
            list.append(record);
        }
    }
    file.close();
    return list;
}
QMap<long,QString> HistoryGather::history()
{
    ls();
    QMap<long,QString> map;
    QStringList files = m_fileMap.keys();
    for(const QString& f : files)
    {
        QList<HistRecord> list = history(f);
        for(HistRecord record : list)
        {
            map[record.timestamp] = f+":"+record.command;
        }
    }
    return map;
}
QMap<long,QString> HistoryGather::history(long t1, long t2)
{
    ls();
    QMap<long,QString> map;
    QStringList files = m_fileMap.keys();
    for(const QString& f : files)
    {
        QList<HistRecord> list = history(f,t1,t2);
        for(HistRecord record : list)
        {
            map[record.timestamp] = f+":"+record.command;
        }
    }
    return map;
}

