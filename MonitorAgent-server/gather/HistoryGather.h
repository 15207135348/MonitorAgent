#ifndef HISTORYGATHER_H
#define HISTORYGATHER_H

#include <QString>
#include <QMap>
#include <QStringList>

struct History
{
    QString start;
    QString stop;
    QString text;
};

class HistoryGather
{
public:
    static HistoryGather* getInstance();
    QStringList ls();
    History history(const QString& filename);
protected:
    HistoryGather();
    static HistoryGather* _instance;
    void init();

private:

};

#endif // HISTORYGATHER_H
