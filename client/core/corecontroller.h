#ifndef CORECONTROLLER_H
#define CORECONTROLLER_H

#include <QObject>
#include <QSharedPointer>

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
};

#endif // CORECONTROLLER_H
