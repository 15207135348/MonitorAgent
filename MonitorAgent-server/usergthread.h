#ifndef USERGTHREAD_H
#define USERGTHREAD_H

#include <QThread>
#include <gather/BaseGather.h>
#include <gather/UserGather.h>

class UserGThread : public QThread
{
    Q_OBJECT
public:
    UserGThread(unsigned int internal, QThread *parent = nullptr);
    void stop();
private:
    bool m_run = true;
    unsigned int m_internal;
    UserGather* m_userGather;
    BaseGather* m_baseGather;
protected:
    void run();
signals:
    void gatherDone(const QMap<QString,QVariant>& map);

};

#endif // USERGTHREAD_H
