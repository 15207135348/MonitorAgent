#ifndef HISTORYGTHREAD_H
#define HISTORYGTHREAD_H

#include <QThread>
#include <gather/HistoryGather.h>
#include <gather/BaseGather.h>

class HistoryGThread : public QThread
{
    Q_OBJECT
public:
    HistoryGThread(unsigned int internal, QThread *parent = nullptr);
    void stop();
private:
    bool m_run = true;
    unsigned int m_internal;
    HistoryGather* m_hisGather;
    BaseGather* m_baseGather;
protected:
    void run();
signals:
    void gatherDone(const QMap<QString,QVariant>& map);

};

#endif // HISTORYGTHREAD_H
