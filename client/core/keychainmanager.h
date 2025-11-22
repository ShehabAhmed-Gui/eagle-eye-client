#ifndef KEYCHAINMANAGER_H
#define KEYCHAINMANAGER_H

#include <QObject>

#include <keychain.h>

class KeychainManager : public QObject
{
    Q_OBJECT
public:
    explicit KeychainManager(QObject *parent = nullptr);

    void writeKey(const QString &key, const QString &value);
    void readKey(const QString &key);

    // To be used to clean up
    void deleteKey(const QString &key);

signals:
    void readyRead(const QString &data);

private:
    QKeychain::WritePasswordJob m_writeJob;
    QKeychain::ReadPasswordJob m_readJob;
    QKeychain::DeletePasswordJob m_deleteJob;
};

#endif // KEYCHAINMANAGER_H
