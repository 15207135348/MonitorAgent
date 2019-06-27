#ifndef DISKINFOGATHER_H
#define DISKINFOGATHER_H
#include <QObject>
class DiskGather
{
public:
    DiskGather(int internal);

    double fsize();

    bool compute();

    double iops();
    double iokbs();

private:
    int m_internal;
    double m_sec;

    bool fill(int i);

    long m_value[2][2];
    double m_usage[2];
};

#endif // DISKINFOGATHER_H
