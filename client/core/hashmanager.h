#ifndef HASHMANAGER_H
#define HASHMANAGER_H

#include <QObject>
#include <QByteArray>
#include <QDebug>
#include <QCoreApplication>
#include <unordered_map>

#include "defs.h"

using ViolationType = appguard::ViolationType;

class HashManager : public QObject
{
    Q_OBJECT

public:
    explicit HashManager(const QString originalPath,
                         QObject *parent = nullptr);

    void activate();

public slots:
    void verifyHashes();

signals:
    void violationDetected(ViolationType type);

private:
    QByteArray calculateHash(const QString &filePath);

    QString m_originalPath;
    QVector<QString> m_files;
    std::unordered_map<QString, QByteArray> m_hashes;
};

#endif // HASHMANAGER_H
