#include "mainconsole.h"

Mainconsole::Mainconsole(int argc, char *argv[], QObject *parent) : QObject(parent)
{
    QMap<QString,QString> map = parse(argc, argv);
    initConfig(map);
    initClient();
    initGather();
    show();
}

QMap<QString,QString> Mainconsole::parse(int argc, char *argv[])
{
    QMap<QString,QString> map;
    for(int i = 1; i!= argc; i+=2)
    {
        QString key = argv[i];
        QString value = argv[i+1];
        if(key == "--ip")
        {
            map["host.address"] = value;
        }
        else if (key == "--port")
        {
            map["host.port"] = value;
        }
        else if (key == "--internal")
        {
            map["gather.internal"] = value;
        }
        else if (key == "--username")
        {
            map["user.name"] = value;
        }
        else if (key == "--selfstart")
        {
            map["self.start"] = value;
        }
    }
    return map;
}

void Mainconsole::initConfig(QMap<QString,QString>& map)
{
    if (map.isEmpty())
    {
        map = ConfigUtil::loadConf();
    }
    m_host = map["host.address"];
    m_port = map["host.port"].toUShort();
    m_internal = map["gather.internal"].toUInt();
    m_username = map["user.name"];
    m_selfstart = map["self.start"] == "true";
    ConfigUtil::saveConf(map);
    ConfigUtil::setSelfStart(m_selfstart);
}
void Mainconsole::initClient()
{
    m_client = new Client(m_host,m_port,m_username);
}
void Mainconsole::initGather()
{
    m_metrics = new MetricsGThread(m_internal);
    connect(m_metrics,SIGNAL(gatherDone(const QMap<QString,QVariant>&)),
            this,SLOT(onGatherDone(const QMap<QString,QVariant>&)));
    m_metrics->start();

    m_history = new HistoryGThread(m_internal);
    connect(m_history,SIGNAL(gatherDone(const QMap<QString,QVariant>&)),
            this, SLOT(onGatherDone(const QMap<QString,QVariant>&)));
    m_history->start();

    m_user = new UserGThread(m_internal);
    connect(m_user, SIGNAL(gatherDone(QMap<QString,QVariant>)),
            this,SLOT(onGatherDone(QMap<QString,QVariant>)));
    m_user->start();
}
void Mainconsole::onGatherDone(const QMap<QString, QVariant> &map)
{
    m_client->send(map);
}

void Mainconsole::show()
{
    printf("host.ip=%s\n",m_host.toStdString().c_str());
    printf("host.port=%d\n",m_port);
    printf("gather.internal=%dms\n",m_internal);
    printf("user.name=%s\n",m_username.toStdString().c_str());
    printf("self.start=%s\n",m_selfstart?"true":"false");
    printf("connection.status=%s\n",m_client->is_connected()?"connected":"disconnected");
    printf("agent.status=running\n");
}
