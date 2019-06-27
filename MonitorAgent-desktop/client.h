#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTimer>
#include <QTcpSocket>

class BaseGather;
class Client : public QTcpSocket
{
    Q_OBJECT
public:
    explicit Client(QString host, quint16 port, QObject* parent=nullptr);
    ~Client();
    void send(const QMap<QString, QVariant> &map);
    void send(const QJsonObject &obj);
    bool is_connected(){
        return m_connected;
    }
    void connectHost();
signals:

public slots:
    void onConnected();
    void onDisConnect();
    void onRecieved();
private:
    BaseGather* m_baseGather;
    QString m_host;
    unsigned short m_port;
    QTimer* m_timer = NULL;
    bool m_connected = false;
};

#endif // CLIENT_H
