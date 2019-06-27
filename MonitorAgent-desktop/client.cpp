#include "client.h"

#include <QFile>
#include <QVariant>
#include <QJsonObject>
#include <QJsonDocument>

#include <util/ConfigUtil.h>
#include <GatherThread.h>
#include <gather/BaseGather.h>
#include <gather/UserGather.h>
#include <gather/HistoryGather.h>

#define RECONNECT_DELAY 5000
#define PROTOCOL_DELIMITER "@end"


Client::Client(QString host, quint16 port, QObject *parent) :
    QTcpSocket(parent)
{
    m_host = host;
    m_port = port;
    connect(this, SIGNAL(connected()),this,SLOT(onConnected()));
    connect(this, SIGNAL(disconnected()),this,SLOT(onDisConnect()));
    connect(this, SIGNAL(readyRead()),this,SLOT(onRecieved()));
    connectHost();
}
Client::~Client()
{
    if(m_timer!=NULL)
    {
        m_timer->stop();
        delete m_timer;
    }
}
void Client::connectHost()
{
    //如果处于断线状态,则连接主机
    if(!m_connected)
    {
        connectToHost(m_host, m_port);
        waitForConnected(1000);
    }
    //判断连接是否成功
    if(state() != ConnectedState && m_timer!=NULL)
    {
        //如果连接失败,开启重连定时器
        m_timer = new QTimer(this);
        connect(m_timer,&QTimer::timeout,[this]{
            connectToHost(m_host, m_port);
            waitForConnected(1000);
        });
        m_timer->start(RECONNECT_DELAY);
    }
}
void Client::onConnected()
{
    //连接成功,关闭重连定时器
    m_connected = true;
    if(m_timer!=NULL)
    {
        m_timer->stop();
        m_timer->deleteLater();
        m_timer = NULL;
    }
    //连接成功后,发送主机的基本信息
    QJsonObject baseInfo = BaseGather::getInstance()->baseInfo();
    QByteArray array = QJsonDocument(baseInfo).toJson().append(PROTOCOL_DELIMITER);
    write(array);
    waitForBytesWritten();
}
void Client::onDisConnect()
{
    m_connected = false;
    m_timer = new QTimer(this);
    connect(m_timer,&QTimer::timeout,[this]{
        connectToHost(m_host, m_port);
        waitForConnected(1000);
    });
    m_timer->start(RECONNECT_DELAY);
}
void Client::onRecieved()
{
    while (bytesAvailable() > 0)
    {
        QJsonObject json = QJsonDocument::fromJson(readAll()).object();
        QString method = json["method"].toString();
        if(method == "logfail")
        {
            QJsonObject obj;
            long t1 = json["t1"].toString().toLong();
            long t2 = json["t2"].toString().toLong();
            QMap<QString,int> map = UserGather::logfail(t1,t2);
            for(auto it=map.begin();it!=map.end();++it)
            {
                obj[it.key()] = it.value();
            }
            obj["mac"] = BaseGather::getInstance()->mac();
            send(obj);
        }
        else if(method == "history")
        {
            QJsonObject obj;
            long t1 = json["t1"].toString().toLong();
            long t2 = json["t2"].toString().toLong();
            QMap<long,QString> map = HistoryGather::getInstance()->history(t1,t2);
            for(auto it=map.begin();it!=map.end();++it)
            {
                obj[QString::number(it.key())] = it.value();
            }
            obj["mac"] = BaseGather::getInstance()->mac();
            send(obj);
        }
    }
}
void Client::send(const QMap<QString, QVariant> &map)
{
    if(m_connected)
    {
        QJsonObject obj = QJsonObject::fromVariantMap(map);
        QByteArray buffer = QJsonDocument(obj).toJson().append(PROTOCOL_DELIMITER);
        write(buffer);
        waitForBytesWritten();
    }
}
void Client::send(const QJsonObject &obj)
{
    if(m_connected)
    {
        QByteArray buffer = QJsonDocument(obj).toJson().append(PROTOCOL_DELIMITER);
        write(buffer);
        waitForBytesWritten();
    }
}
