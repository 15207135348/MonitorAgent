#ifndef MAINCONSOLE_H
#define MAINCONSOLE_H

#include <QObject>
#include <QString>
#include <QMap>
#include <client.h>
#include <metricsgthread.h>
#include <historygthread.h>
#include <usergthread.h>
#include <util/ConfigUtil.h>
#include <util/Fileutil.h>

class Mainconsole : public QObject
{
    Q_OBJECT
public:
    explicit Mainconsole(int argc, char *argv[], QObject *parent = nullptr);

    QMap<QString,QString> parse(int argc, char *argv[]);
    void initGather();
    void initClient();
    void initConfig(QMap<QString, QString> &map);
    void show();

private:

    QString m_username;
    int m_internal;
    QString m_host;
    quint16 m_port;
    bool m_selfstart;

    Client* m_client;
    MetricsGThread* m_metrics;
    HistoryGThread* m_history;
    UserGThread* m_user;

signals:

public slots:
    void onGatherDone(const QMap<QString, QVariant> &map);
};

#endif // MAINCONSOLE_H
