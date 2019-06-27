#ifndef DISKINFOGATHER_H
#define DISKINFOGATHER_H
#include <QObject>
class DiskGather
{
public:
    DiskGather(int internal);

    double fsize();

    bool compute();

    double ips();
    double ops();
    double ikbs();
    double okbs();

private:
    int m_internal;
    double m_sec;

    bool fill(int i);

    long m_value[2][4];
    double m_usage[4];
};

#endif // DISKINFOGATHER_H
