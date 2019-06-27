#ifndef RAMINFOGATHER_H
#define RAMINFOGATHER_H

class RamGather
{
public:
    RamGather();
    bool compute();

    double buffers();
    double cached();
    double actual();
    double usage();


private:
    long m_value[4];
    double m_usage[4];
    bool fill();
};

#endif // RAMINFOGATHER_H
