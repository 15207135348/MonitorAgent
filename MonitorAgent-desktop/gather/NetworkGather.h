#ifndef NETINFOGATHER_H
#define NETINFOGATHER_H

class NetworkGather
{

public:
    NetworkGather(int internal);

    double kbs();

private:
    long traffic();
    int m_internal;
    double m_divisor;
};

#endif // NETINFOGATHER_H
