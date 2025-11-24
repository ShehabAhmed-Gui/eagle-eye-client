/* Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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
    return R"(\\.\pipe\eagleeye)";
#endif
}
