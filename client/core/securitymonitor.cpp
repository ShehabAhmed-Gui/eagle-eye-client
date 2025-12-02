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

namespace {
Logger logger("SecurityMonitor");
}

SecurityMonitor::SecurityMonitor(QSharedPointer<HashManager> hashManager,
                                 QObject *parent)
    : QThread{parent}
    , m_hashManager(hashManager)
{
    // Seed the random number generator once at program start
    srand(static_cast<unsigned int>(time(nullptr)));

    connect(m_hashManager.get(), &HashManager::violationDetected, this, &SecurityMonitor::onViolationDetected);

    QJsonObject obj;
    obj.insert("allowed", true);
    setToken(obj);

    m_processMonitor.reset(new ProcessMonitor());
}

void SecurityMonitor::activate()
{
    logger.debug() << "Activiating security monitor";
    m_hashManager->activate();
    m_running = true;
    startSecurityLoop();
}

void SecurityMonitor::setToken(const QJsonObject &token)
{
    m_token = token;
}

QJsonObject SecurityMonitor::token()
{
    return m_token;
}

QString SecurityMonitor::getViolationDetails()
{
    return m_violationDetails;
}

void SecurityMonitor::onViolationDetected(eagle_eye::ViolationType type)
{
    QString details;
    switch (type) {
    case eagle_eye::ViolationType::NoViolation:
        logger.debug() << "No violation is detected";
        break;
    case eagle_eye::ViolationType::HashViolation:
        details = "Hash violation is detected";
        m_violationDetails = details;
        logger.critical() << details;
        break;
    case eagle_eye::ViolationType::DebuggerViolation:
        details = "Process is running in a debugger";
        m_violationDetails = details;
        logger.critical() << details;
        break;
    case eagle_eye::ViolationType::EagleEyeRunningInADebugger:
        details = "EagleEye process is running in a debugger";
        m_violationDetails = details;
        logger.critical() << details;
        break;
    case eagle_eye::DLLInjectionViolation:
        details = "An injected DLL is detected";
        m_violationDetails = details;
        logger.critical() << details;
        break;
    default:
        break;
    }

    // Only take action if there is a violation
    if (type != eagle_eye::NoViolation) {
        QJsonObject obj;
        obj.insert("allowed", false);
        obj.insert("details", details);
        setToken(obj);

        // Stop the check loop
        m_running = false;
        emit integrityViolationDetected();
    }
}

void SecurityMonitor::startSecurityLoop()
{
    while (m_running) {
        auto now = QDateTime::currentMSecsSinceEpoch();

        if (now > m_nextFastCheck) {
            fastCheck();
            m_nextFastCheck = now + (rand() % 501);
        }

        if (now > m_nextMediumCheck) {
            mediumCheck();
            m_nextMediumCheck = now + (rand() % 3001);
        }

        if (now > m_nextSlowCheck) {
            slowCheck();
            m_nextSlowCheck = now + (rand() % 10001);
        }

        QThread::msleep(50);
    }
}

void SecurityMonitor::fastCheck()
{
    eagle_eye::ViolationType result = m_processMonitor->CheckForDebugger();
    if (result != ViolationType::NoViolation) {
        onViolationDetected(result);
    }
}

void SecurityMonitor::mediumCheck()
{
    eagle_eye::ViolationType result = m_processMonitor->CheckForDllInjection();
    if (result != ViolationType::NoViolation) {
        onViolationDetected(result);
    }
}

void SecurityMonitor::slowCheck()
{
    m_hashManager->onSecurityCheck();
}

void SecurityMonitor::run()
{
    activate();
}
