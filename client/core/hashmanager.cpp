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

#include "hashmanager.h"
#include "hash.h"
#include "filesmanager.h"
#include "logger.h"
#include "utils.h"

namespace {
Logger logger("HashManager");
}

HashManager::HashManager(const QString originalPath,
                         QSharedPointer<KeychainManager> keychainManger,
                         QObject *parent)
    : QObject{parent}
    , m_originalPath(originalPath)
    , m_keychainManger(keychainManger)
{
    m_key = Utils::getSecureKey();
}

void HashManager::activate()
{
    QVector<QString> files = FilesManager::getProgramFiles(m_originalPath);
    m_files = files;

    // We store files hashes
    // into secure local storage with a random-generated key
    // on activate (which is called on service startup).

    // Only one value is allowed under a key
    // so we use the index to make a new key for each hash.
    int index = 0;
    for (QString &file : files) {
        QByteArray hash = calculateHash(file);
        if (hash.isNull()) {
            logger.error() << "Could not calculate hash for:" << file;
            return;
        }

        index++;
        const QString key = m_key + QString::number(index);
        m_keychainManger->writeKey(key, hash);
    }

    logger.debug() << "Calculated hashes for" << index;
}

// We calculate the hash by streaming the file chunk by chunk to not hog memory
// Chunk size is arbitrary, can be anything
QByteArray HashManager::calculateHash(const QString &filePath)
{
    // Maximum size of a chunk in bytes
    const int FILE_CHUNK_SIZE = 1000000;

    QFile file(filePath);
    if (!file.open(QIODevice::ExistingOnly | QIODevice::ReadOnly)) {
        logger.error() << "Failed to open:" << file.fileName() << file.errorString();
        return QByteArray();
    }
    qint64 fileSize = file.size();
    
    hmac_sha256_state sha;
    hmac_sha256_init(&sha, fileSize);

    QDataStream dataStream(&file);
    std::array<char, FILE_CHUNK_SIZE> chunk;
    int chunk_len = 0;
    
    while (dataStream.atEnd() == false)
    {
        chunk_len = dataStream.readRawData(chunk.data(), FILE_CHUNK_SIZE);
        hmac_sha256_update(&sha, chunk.data(), chunk_len);
    }

    hmac_sha256_finalize(&sha);

    return QByteArray(/*reinterpret_cast<const char*>(sha.hash.data()), sha.hash.size() * sizeof(uint32_t)*/);
}

void HashManager::retrieveStoredHashes()
{
    QVector<QByteArray> result;
    m_pendingHashes = m_files.size();

    for (int i = 0; i < m_files.size(); ++i) {
        const QString key = m_key + QString::number(i);
        m_keychainManger->readKey(key);
    }
}

void HashManager::verifyHashes()
{
    for (int i = 0; i < m_files.size(); ++i) {
        const QString file = m_files[i];
        if (calculateHash(file) != m_storedHashes[i]) {
            logger.warning() << "Found a violation in file:" << file;
            emit violationDetected(eagle_eye::ViolationType::HashViolation);
            return;
        }
    }
}

void HashManager::onReadyRead(const QString &data)
{
    m_storedHashes.push_back(data.toUtf8());
    m_pendingHashes--;

    if (m_pendingHashes == 0) {
        verifyHashes();
    }
}

void HashManager::onSecurityCheck()
{
    retrieveStoredHashes();
}
