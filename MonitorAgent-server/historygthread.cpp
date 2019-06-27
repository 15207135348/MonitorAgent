#include "historygthread.h"

#include <QDateTime>
#include <QVariant>
#include <sys/inotify.h>
#include <unistd.h>

HistoryGThread::HistoryGThread(unsigned int internal, QThread *parent) : QThread(parent)
{
    m_internal = internal;
    m_hisGather = HistoryGather::getInstance();
    m_baseGather = BaseGather::getInstance();
}
void HistoryGThread::stop()
{
    m_run = false;
}

void HistoryGThread::run()
{
    while (m_run) {

        qint64 t1 = QDateTime::currentMSecsSinceEpoch();
        //汇总信息包
        QMap<QString,QVariant> map;
        //时间戳
        map["H"] = "4";
        map["T"] = t1/1000;
        map["M"] = m_baseGather->mac();
        QStringList filenames = m_hisGather->ls();
        for(const QString& filename : filenames)
        {
            History hist = m_hisGather->history(filename);
            QString key = filename.mid(0,filename.lastIndexOf(".")) + "[" + hist.start+ "-" + hist.stop + "]";
            map[key] = hist.text;
        }
        //触发信息槽
        emit gatherDone(map);
        qint64 t2 = QDateTime::currentMSecsSinceEpoch();
        int ms = m_internal - (t2-t1);
        if(ms > 0)
        {
          QThread::msleep(ms);
        }
    }
}
