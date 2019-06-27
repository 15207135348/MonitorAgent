#ifndef HISTORYGATHER_H
#define HISTORYGATHER_H

#include <QString>
#include <QMap>
#include <QStringList>

struct HistRecord
{
    long timestamp;
    QString command;
};

class HistoryGather
{
public:
    static HistoryGather* getInstance();

    QStringList ls();

    QList<HistRecord> history(const QString& filename);
    QList<HistRecord> history(const QString& filename, long t1, long t2);

    QMap<long, QString> history();
    QMap<long, QString> history(long t1, long t2);




protected:
    HistoryGather();
    void init();
    static HistoryGather* _instance;
private:
    QMap<QString,bool> m_fileMap;
};

#endif // HISTORYGATHER_H
