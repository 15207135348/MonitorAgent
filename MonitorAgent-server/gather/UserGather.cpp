#include "UserGather.h"

#include <unistd.h>
#include <fcntl.h>
#include <utmp.h>
#include <util/Fileutil.h>

#define BTMP_FILE "/var/log/btmp"

QList<User> UserGather::m_btmps;
QList<User> UserGather::m_utmps;

UserGather::UserGather()
{

}
bool UserGather::is_utmp_modify()
{
    return FileUtil::hasModify(UTMP_FILE);
}
bool UserGather::is_btmp_modify()
{
    return FileUtil::hasModify(BTMP_FILE);
}
/*
 * 获取所有登陆这台电脑的用户信息
 * 1、登陆的用户名
 * 2、登陆的设备名
 * 3、用户的IP地址
 * 4、登陆的进程号
 * 5、登陆的时间
 */
void UserGather::load_utmp()
{
    if(!is_utmp_modify())
    {
        return;
    }
    /*声明utmp结构变量*/
    struct utmp pointer;
    /*打开的utmp文件存放在这里*/
    int utmpfd;
    //每次读 sizeof(utmp) 字节
    int re=sizeof(pointer);
    //打开utmp文件
    if((utmpfd = open(UTMP_FILE, O_RDONLY)) == 1)
    {
        return;
    }
    m_utmps.clear();
    /*读取文件中的内容*/
    while(read(utmpfd, &pointer, re) == re)
    {
        if(pointer.ut_type == USER_PROCESS)
        {
            User user;
            user.name = pointer.ut_name;
            user.tty = pointer.ut_line;
            user.host = pointer.ut_host;
            user.pid = pointer.ut_pid;
            user.time = pointer.ut_time;
            m_utmps.append(user);
        }
    }
    close(utmpfd);

}
/*
 * 获取所有登录失败的用户信息
 * 1、登陆的用户名
 * 2、登陆的设备名
 * 3、用户的IP地址
 * 4、登陆的进程号
 * 5、登陆的时间
 */
void UserGather::load_btmp()
{
    if(!is_btmp_modify())
    {
        return;
    }
    /*声明utmp结构变量*/
    struct utmp pointer;
    /*打开的utmp文件存放在这里*/
    int btmpfd;
    //每次读 sizeof(utmp) 字节
    int re=sizeof(pointer);
    //打开utmp文件
    if((btmpfd = open(BTMP_FILE, O_RDONLY)) == 1)
    {
        return;
    }
    m_btmps.clear();
    /*读取文件中的内容*/
    while(read(btmpfd, &pointer, re) == re)
    {
        if(pointer.ut_type == LOGIN_PROCESS)
        {
            User user;
            user.name = pointer.ut_name;
            user.tty = pointer.ut_line;
            user.host = pointer.ut_host;
            user.pid = pointer.ut_pid;
            user.time = pointer.ut_time;
            m_btmps.append(user);
        }
    }
    close(btmpfd);
}
QMap<QString,int> UserGather::logfail()
{
    load_btmp();
    QMap<QString,int> map;
    for(const User& user : m_btmps)
    {
        map[user.host] += 1;
    }
    return map;
}
QMap<QString,int> UserGather::logfail(long t1, long t2)
{
    load_btmp();
    QMap<QString,int> map;
    for(const User& user : m_btmps)
    {
        if(user.time>=t1 && user.time<=t2)
        {
            map[user.host] += 1;
        }
    }
    return map;
}
int UserGather::merge(const QMap<QString,int>& map)
{
    int count = 0;
    QList<int> values = map.values();
    for(int v : values)
    {
        count += v;
    }
    return count;
}


