#ifndef CPUINFOGATHER_H
#define CPUINFOGATHER_H

class CpuGather
{
public:
    CpuGather(int internal);
    bool compute();

    double user();
    double nice();
    double system();
    double idle();
    double iowait();
    double irq();
    double softirq();
    double usage();

private:
    bool fill(int i);
    long m_times[2][7];
    double m_usage[7];
    int m_internal;
};

#endif // CPUINFOGATHER_H
