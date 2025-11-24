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

#include "securitymonitor.h"
#include "logger.h"

#define VERIFY_TIME_MSEC 2000

namespace {
Logger logger("SecurityMonitor");
}

SecurityMonitor::SecurityMonitor(QSharedPointer<HashManager> hashManager,
                                 QObject *parent)
    : QObject{parent}
    , m_hashManager(hashManager)
{
    connect(m_hashManager.get(), &HashManager::violationDetected, this, &SecurityMonitor::onViolationDetected);

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &SecurityMonitor::integrityCheck);
    logger.debug() << "Security monitor init";

    QJsonObject obj;
    obj.insert("allowed", true);
    setToken(obj);

    m_processMonitor.reset(new ProcessMonitor());

    activate();
}

void SecurityMonitor::activate()
{
    m_hashManager->activate();

    m_timer->setSingleShot(false);
    m_timer->start(VERIFY_TIME_MSEC);
}

void SecurityMonitor::setToken(const QJsonObject &token)
{
    m_token = token;
}

QJsonObject SecurityMonitor::token()
{
    return m_token;
}

void SecurityMonitor::integrityCheck()
{
    eagle_eye::ViolationType result = m_processMonitor->run();
    if (result != ViolationType::NoViolation) {
        logger.critical() << "Violation detected";
        onViolationDetected(result);
        return;
    }
    else {
        logger.debug() << "No violation detected";
    }

    m_hashManager->onSecurityCheck();
}

void SecurityMonitor::onViolationDetected(eagle_eye::ViolationType type)
{
    m_timer->stop();

    QJsonObject obj;
    obj.insert("allowed", false);

    QString details;

    switch (type) {
    case eagle_eye::ViolationType::HashViolation:
        details = "Hash violation is detected";
        break;
    case eagle_eye::ViolationType::DebuggerViolation:
        details = "Process is running in a debugger";
        break;
    default:
        break;
    }

    obj.insert("details", details);
    setToken(obj);

    emit integrityViolationDetected();
}
