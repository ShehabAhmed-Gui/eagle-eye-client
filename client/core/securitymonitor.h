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

#ifndef SECURITYMONITOR_H
#define SECURITYMONITOR_H

#include <QObject>
#include <QTimer>
#include <QSharedPointer>
#include <QJsonObject>

#include "hashmanager.h"
#include "processmonitor.h"
#include "defs.h"

class SecurityMonitor : public QObject
{
    Q_OBJECT
public:
    explicit SecurityMonitor(QSharedPointer<HashManager> hashManager,
                             QObject *parent = nullptr);

    void activate();

    void setToken(const QJsonObject &token);
    QJsonObject token();

signals:
    void integrityViolationDetected();

public slots:
    void integrityCheck();
    void onViolationDetected(eagle_eye::ViolationType type);

private:
    QJsonObject m_token;
    QSharedPointer<HashManager> m_hashManager;
    QSharedPointer<ProcessMonitor> m_processMonitor;
    QTimer *m_timer;
};

#endif // SECURITYMONITOR_H
