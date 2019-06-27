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

    static void append(const QString& filename, const QMap<QString, QVariant> map);

private:
    static QMap<QString,long> modify_time;
};

#endif // FILEUTIL_H
