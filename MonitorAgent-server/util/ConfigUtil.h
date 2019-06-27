#ifndef DOMDOCUMENT_H
#define DOMDOCUMENT_H
#include <QObject>

class ConfigUtil
{
public:
    ConfigUtil();
    static QMap<QString, QString> loadConf();
    static void saveConf(const QMap<QString, QString> &map);
    static void setSelfStart(bool selfstart);
};



#endif // DOMDOCUMENT_H
