#include "iptables.h"
#include  <QProcess>

Iptables::Iptables(QObject *parent) : QObject(parent)
{

}

bool Iptables::disable(const QString& ip)
{
    return QProcess::execute(QString("iptables -I INPUT -s %1 -j DROP").arg(ip)) == 0;
}
bool Iptables::enable(const QString& ip)
{
    return  QProcess::execute(QString("iptables -D INPUT -s %1 -j DROP").arg(ip)) == 0;
}
