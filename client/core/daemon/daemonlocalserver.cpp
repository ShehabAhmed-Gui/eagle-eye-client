#include "daemonlocalserver.h"
#include "../logger.h"
#include "../securitymonitor.h"

namespace {
Logger logger("DaemonLocalServer");
}

DaemonLocalServer::DaemonLocalServer(QSharedPointer<SecurityMonitor> securityMonitor,
                                     QObject *parent)
    : QObject{parent}
    , m_securityMonitor(securityMonitor)
{
    m_localServer = new QLocalServer(this);
    connect(m_localServer, &QLocalServer::newConnection, this, &DaemonLocalServer::onNewConnection);
}

void DaemonLocalServer::start()
{
    m_localServer->setSocketOptions(QLocalServer::WorldAccessOption);
    m_localServer->listen(getDaemonPath());
    logger.debug() << "Started daemon local server.";
}

void DaemonLocalServer::onNewConnection()
{
    logger.debug() << "New pipe connection";
    QLocalSocket *socket = m_localServer->nextPendingConnection();
    if (!socket) {
        logger.error() << "Invalid local socket pointer";
        return;
    }

    m_connection = new DaemonConnection(m_securityMonitor, socket, this);
}

QString DaemonLocalServer::getDaemonPath() const
{
#if defined (Q_OS_WIN)
    return R"(\\.\pipe\appguard)";
#endif
}
