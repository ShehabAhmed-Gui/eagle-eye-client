#include "securitymonitor.h"
#include "processmonitor.h"
#include "logger.h"

#define VERIFY_TIME_MSEC 10000

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

    QJsonObject obj;
    obj.insert("allowed", true);
    setToken(obj);
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
    eagle_eye::ViolationType result = ProcessMonitor::run();
    if (result != ViolationType::NoViolation) {
        logger.critical() << "Debugger violation detected";
        onViolationDetected(result);
        return;
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
