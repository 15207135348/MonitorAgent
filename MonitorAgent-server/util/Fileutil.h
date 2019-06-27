#ifndef FILEUTIL_H
#define FILEUTIL_H

#include <QMap>
#include <QString>
#include <QVariant>

class FileUtil
{
public:
    FileUtil();
    static bool hasModify(const QString& filename);

private:
    static QMap<QString,long> modify_time;
};

#endif // FILEUTIL_H
