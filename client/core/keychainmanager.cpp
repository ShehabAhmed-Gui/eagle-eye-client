#include "keychainmanager.h"
#include "logger.h"

namespace {
Logger logger("KeychainManager");
}

KeychainManager::KeychainManager(QObject *parent)
    : QObject{parent}
    , m_writeJob("com.eagleeye.eagleeye", this)
    , m_readJob("com.eagleeye.eagleeye", this)
    , m_deleteJob("com.eagleeye.eagleeye", this)
{
    m_writeJob.setAutoDelete(false);
    m_readJob.setAutoDelete(false);
    m_deleteJob.setAutoDelete(false);
}

void KeychainManager::writeKey(const QString &key, const QString &value)
{
    m_writeJob.setKey(key);

    connect(&m_writeJob, &QKeychain::WritePasswordJob::finished, [=]() {
        if (m_writeJob.error()) {
            logger.critical() << "Write job failed with error:" << m_writeJob.errorString();
            return;
        }
        logger.debug() << "Write job succeed";
    });

    m_writeJob.setTextData(value);
    m_writeJob.start();
}

void KeychainManager::readKey(const QString &key)
{
    m_readJob.setKey(key);

    connect(&m_readJob, &QKeychain::WritePasswordJob::finished, [=]() {
        if (m_readJob.error()) {
//            logger.critical() << "Read job failed with error:" << m_readJob.errorString();
            return;
        }

        logger.debug() << "Read job succeed";
        emit readyRead(m_readJob.textData());
    });

    m_readJob.start();
}

void KeychainManager::deleteKey(const QString &key)
{
    m_deleteJob.setKey(key);

    connect(&m_deleteJob, &QKeychain::DeletePasswordJob::finished, [=]() {
        if (m_deleteJob.error()) {
            logger.critical() << "Delete job failed with error:" << m_deleteJob.errorString();
            return;
        }

        logger.debug() << "Delete job succeed";
    });
}
