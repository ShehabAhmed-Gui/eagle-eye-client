#include "daemonconnection.h"
#include "../logger.h"

namespace {
Logger logger("DaemonConnection");
}

DaemonConnection::DaemonConnection(QSharedPointer<SecurityMonitor> securityMonitor,
                                   QLocalSocket *socket,
                                   QObject *parent)
    : QObject{parent}
    , m_securityMonitor(securityMonitor)
    , m_socket(socket)
{
    connect(socket, &QLocalSocket::readyRead, this, &DaemonConnection::onReadyRead);
}

void DaemonConnection::onReadyRead()
{
    QByteArray command = m_socket->readAll();
    parseCommand(command);
}

bool DaemonConnection::compareAppId(const QString &appId)
{
    const QVector<QString> exeFiles = FilesManager::getExeFiles(QCoreApplication::applicationDirPath());

    for (const QString &file : exeFiles) {
        const QFileInfo fileInfo(file);
        const QString cleanFileName = fileInfo.baseName().toLower();
        const QString appName = appId.split(".").last().toLower();

        if (cleanFileName == appName) {
            return true;
        }
    }

    return false;
}

void DaemonConnection::parseCommand(const QByteArray &command)
{
    // Parse commands as json
    QJsonDocument doc = QJsonDocument::fromJson(command);
    QJsonObject obj = doc.object();

    const QString cmd = obj.value("cmd").toString();
    logger.debug() << "Parsing command:" << cmd;

    if (cmd == "token_request") {
        const QString appId = obj.value("app_id").toString();

        if (!compareAppId(appId)) {
            logger.error() << "Received incorrect app id";
            return;
        }

        QJsonObject obj = m_securityMonitor->token();
        write(obj);
        return;
    }
}

void DaemonConnection::write(const QJsonObject &obj)
{
    m_socket->write(QJsonDocument(obj).toJson(QJsonDocument::Compact));
    m_socket->write("\n");
}
