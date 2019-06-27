#include "usergthread.h"
#include <QDateTime>
#include <QVariant>

UserGThread::UserGThread(unsigned int internal, QThread *parent) : QThread(parent)
{
    m_internal = internal;
    m_baseGather = BaseGather::getInstance();
    m_userGather = new UserGather();
}
void UserGThread::run()
{
    while (m_run) {
        qint64 t1 = QDateTime::currentMSecsSinceEpoch();
        //汇总信息包
        QMap<QString,QVariant> map;
        //时间戳
        map["H"] = "3";
        map["T"] = t1/1000;
        map["M"] = m_baseGather->mac();
        QMap<QString, int> logfailMap = m_userGather->logfail(t1 - m_internal, t1);
        for(auto it = logfailMap.begin(); it!=logfailMap.end(); ++it)
        {
            map[it.key()] = it.value();
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
void UserGThread::stop()
{
    m_run = false;
}
