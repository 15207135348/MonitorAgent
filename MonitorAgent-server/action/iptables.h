#ifndef DISABLEIP_H
#define DISABLEIP_H

#include <QObject>

class Iptables : public QObject
{
    Q_OBJECT
public:
    explicit Iptables(QObject *parent = nullptr);

    static bool disable(const QString& ip);
    static bool enable(const QString& ip);

signals:

public slots:
};

#endif // DISABLEIP_H
