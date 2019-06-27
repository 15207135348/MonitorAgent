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
