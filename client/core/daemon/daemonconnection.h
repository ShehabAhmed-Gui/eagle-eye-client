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
    void write(const QJsonObject &obj);

public slots:
    void onReadyRead();

private:
    bool compareAppId(const QString &appId);
    void parseCommand(const QByteArray &command);

    QLocalSocket *m_socket;
    QSharedPointer<SecurityMonitor> m_securityMonitor;

    bool connected = false;
};

#endif // DAEMONCONNECTION_H
