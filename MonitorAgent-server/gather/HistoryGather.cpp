#include "HistoryGather.h"

#include <QString>
#include <QFile>
#include <QDir>
#include <QTextStream>

#define FILE_PATH QString("/var/log/.history/%1")

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
    "if [ ! -d /var/log/.history ]\n"
    "then\n"
    "        mkdir /var/log/.history\n"
    "        chmod 777 /var/log/.history\n"
    "fi\n"
    "DT=`date +%Y%m%d_%H%M%S`\n"
    "export HISTFILE=\"/var/log/.history/${LOGNAME}@${USER_IP}.$DT\"\n"
    "export HISTFILESIZE=4096\n"
    "export HISTTIMEFORMAT=\"[%F %T][`whoami`][${USER_IP}]\" \n"
    "chmod 600 /var/log/.history/*@* 2>/dev/null\n";
    file.write(text);
    file.close();
}
QStringList HistoryGather::ls()
{
    QDir dir("/var/log/.history/");
    QStringList filters;
    filters << QString("*@*");
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    dir.setNameFilters(filters);
    return dir.entryList();
}

History HistoryGather::history(const QString &filename)
{
    History hist;
    QFile file(FILE_PATH.arg(filename));
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return hist;
    }
    QTextStream in(&file);
    QString time = in.readLine();
    QString command = in.readLine();
    hist.start = time.mid(1);
    while(time != NULL)
    {
        hist.stop = time.mid(1);
        hist.text = hist.text.append(hist.stop).append(":").append(command).append(";");
        time = in.readLine();
        command = in.readLine();
    }
    file.close();
    file.remove();
    return hist;
}
