#ifndef DAEMONCONNECTION_H
#define DAEMONCONNECTION_H

#include <QObject>
#include <QByteArray>
#include <QLocalSocket>
#include <QJsonDocument>
#include <QJsonObject>

#include "../securitymonitor.h"
#include "../filesmanager.h"

class DaemonConnection : public QObject
{
    Q_OBJECT
public:
    explicit DaemonConnection(QSharedPointer<SecurityMonitor> securityMonitor,
                              QLocalSocket *socket,
                              QObject *parent = nullptr);

public slots:
    void onReadyRead();

private:
    bool compareAppId(const QString &appId);
    void parseCommand(const QByteArray &command);
    void write(const QJsonObject &obj);

    QLocalSocket *m_socket;
    QSharedPointer<SecurityMonitor> m_securityMonitor;
};

#endif // DAEMONCONNECTION_H
