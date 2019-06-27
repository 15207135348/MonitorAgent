#include "HistoryGather.h"

#include <QString>
#include <QFile>
#include <QDir>

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
    "if [ ! -d /var/monitor-agent/log/console ]\n"
    "then\n"
    "        mkdir -p /var/monitor-agent/log/console\n"
    "        chmod 777 /var/monitor-agent/log/console\n"
    "fi\n"
    "DT=`date +%Y%m%d_%H%M%S`\n"
    "export HISTFILE=\"/var/monitor-agent/log/console/${LOGNAME}@${USER_IP}.$DT\"\n"
    "export HISTFILESIZE=4096\n"
    "export HISTTIMEFORMAT=\"[%F %T][`whoami`][${USER_IP}]\" \n"
    "chmod 600 /var/monitor-agent/log/console/*@* 2>/dev/null\n";
    file.write(text);
    file.close();
}
