#ifndef HISTORYGATHER_H
#define HISTORYGATHER_H

class HistoryGather
{
public:
    static HistoryGather* getInstance();
protected:
    HistoryGather();
    static HistoryGather* _instance;
    void init();

};

#endif // HISTORYGATHER_H
