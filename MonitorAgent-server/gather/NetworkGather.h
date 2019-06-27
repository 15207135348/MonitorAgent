#ifndef NETINFOGATHER_H
#define NETINFOGATHER_H

class NetworkGather
{

public:
    NetworkGather(int internal);

    bool compute();

    double ikbs();
    double okbs();


private:

    bool fill(int i);
    int m_internal;
    double m_divisor;
    long m_value[2][2];
    double m_usage[2];
};

#endif // NETINFOGATHER_H
