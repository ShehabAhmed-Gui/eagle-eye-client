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
            logger.critical() << "Read job failed with error:" << m_readJob.errorString();
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

    m_deleteJob.setKey(key);
    m_deleteJob.start();
}
