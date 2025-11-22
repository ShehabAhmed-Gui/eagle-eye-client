#ifndef DAEMONLOCALSERVER_H
#define DAEMONLOCALSERVER_H

#include <QObject>
#include <QLocalServer>
#include <QSharedPointer>

#include "daemonconnection.h"
#include "../securitymonitor.h"

class DaemonLocalServer : public QObject
{
    Q_OBJECT
public:
    explicit DaemonLocalServer(QSharedPointer<SecurityMonitor> securityMonitor,
                               QObject *parent = nullptr);

    void start();

public slots:
    void onNewConnection();

private:
    QString getDaemonPath() const;

    QSharedPointer<SecurityMonitor> m_securityMonitor;

    DaemonConnection *m_connection;
    QLocalServer *m_localServer;
};

#endif // DAEMONLOCALSERVER_H
