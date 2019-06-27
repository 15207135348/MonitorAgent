#ifndef USERINFOGATHER_H
#define USERINFOGATHER_H

#include <QList>
#include <QMap>

struct User
{
    QString host;
    QString name;
    QString tty;
    int pid;
    long time;
};

class UserGather
{

public:
    UserGather();
    static QMap<QString,int> logfail();
    static QMap<QString,int> logfail(long t1, long t2);

private:
    static QList<User> m_btmps;
    static QList<User> m_utmps;
    static void load_btmp();
    static void load_utmp();
    static bool is_utmp_modify();
    static bool is_btmp_modify();
    static int merge(const QMap<QString,int>& map);
};

#endif // USERINFOGATHER_H
