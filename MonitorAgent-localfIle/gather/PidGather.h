#ifndef PIDGATHER_H
#define PIDGATHER_H
#include <QString>
#include <QMap>

class PidGather
{
private:

    int m_internal;
    quint64 m_totalMem;
    long m_times[2][7];

    QList<QString> pids();
    QString pidStatus(const QString& pid);
    QList<long> pidState(const QString& pid);
    QMap<QString,QList<long>> pidState(const QList<QString>& pids);
    QString pidInfo(const QList<QString>& pids,
                    const QMap<QString,QList<long>>& pidState1,
                    const QMap<QString,QList<long>>& pidState2, double dt);
    bool fill(int i);

public:
    PidGather(int internal, quint64 totalMem);
    QString pidInfo();

};

#endif // PIDGATHER_H
