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

#ifndef CORECONTROLLER_H
#define CORECONTROLLER_H

#include <QObject>
#include <QSharedPointer>

#include "hashmanager.h"
#include "daemon/daemonlocalserver.h"
#include "securitymonitor.h"

class CoreController : public QObject
{
    Q_OBJECT
public:
    explicit CoreController(QObject *parent = nullptr);
    void init();

public slots:
    void onViolationDetected();

private:
    QString m_originalPath;
    bool killProcess(const QString &fileName);

    QSharedPointer<HashManager> m_hashManager;
    QSharedPointer<KeychainManager> m_keychainManger;
    QSharedPointer<SecurityMonitor> m_securityMonitor;
    DaemonLocalServer* m_daemonServer;
};

#endif // CORECONTROLLER_H
