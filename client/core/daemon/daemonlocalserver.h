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

    bool isConnected();

signals:
    void mainProcessNotConnected();

public slots:
    void onNewConnection();
    void onViolationDetected();

private:
    QString getDaemonPath() const;

    bool connected = false;

    QSharedPointer<SecurityMonitor> m_securityMonitor;

    DaemonConnection *m_connection;
    QLocalServer *m_localServer;
};

#endif // DAEMONLOCALSERVER_H
