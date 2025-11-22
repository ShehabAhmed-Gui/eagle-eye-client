#ifndef SECURITYMONITOR_H
#define SECURITYMONITOR_H

#include <QObject>
#include <QTimer>
#include <QSharedPointer>
#include <QJsonObject>

#include "hashmanager.h"
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
    QTimer *m_timer;
};

#endif // SECURITYMONITOR_H
