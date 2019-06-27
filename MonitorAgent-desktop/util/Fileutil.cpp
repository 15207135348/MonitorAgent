#include "Fileutil.h"

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <QFile>
#include <QDateTime>

QMap<QString,long> FileUtil::modify_time;

FileUtil::FileUtil()
{

}
bool FileUtil::hasModify(const QString& filename)
{
    int fd;
    struct stat buf;
    if((fd = open(filename.toStdString().c_str(), O_RDONLY)) == 1)
    {
        return false;
    }
    fstat(fd, &buf);
    close(fd);
    long cur_time=buf.st_mtime;
    long last_time = modify_time[filename];
    if(cur_time == last_time)
    {
        return false;
    }
    modify_time[filename] = cur_time;
    return true;
}

void FileUtil::append(const QString& filename, const QMap<QString,QVariant> map)
{
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        return;
    }
    QString time = QDateTime::fromMSecsSinceEpoch(map["time"].toLongLong()).toString("yyyy-MM-dd hh:mm:ss");
    QString line = time.append(",")
            .append(QString::number(map["user"].toDouble())).append(",")
            .append(QString::number(map["system"].toDouble())).append(",")
            .append(QString::number(map["iowait"].toDouble())).append(",")
            .append(QString::number(map["buffers"].toDouble())).append(",")
            .append(QString::number(map["cached"].toDouble())).append(",")
            .append(QString::number(map["actual"].toDouble())).append(",")
            .append(QString::number(map["disk"].toDouble())).append(",")
            .append(QString::number(map["iops"].toDouble())).append(",")
            .append(QString::number(map["net"].toDouble())).append("\n");
    file.write(line.toUtf8());
    file.flush();
    file.close();
}
