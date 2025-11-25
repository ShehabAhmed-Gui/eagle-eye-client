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

#include "hashconfigmanager.h"

namespace {
Logger logger("HashManager");
}

HashManager::HashManager(QObject *parent)
    : QObject{parent}
{
}

void HashManager::activate()
{
    QVector<QString> files = FilesManager::getProgramFiles(Utils::getPrimaryAppPath());
    m_files = files;

    QVector<QByteArray> hashes;

    for (QString &file : files) {
        QByteArray hash = calculateHash(file);
        if (hash.isNull()) {
            logger.error() << "Could not calculate hash for:" << file;
            return;
        }

        hashes.push_back(hash);
    }

    logger.debug() << "Calculated hashes for" << files.size() << "files";

    // Store hashes into json
    HashConfigManager::storeHashes(hashes);
}

// We calculate the hash by streaming the file chunk by chunk to not hog memory.
// Chunk size is arbitrary, can be anything
QByteArray HashManager::calculateHash(const QString &filePath)
{
    // Maximum size of a chunk in bytes
    const int FILE_CHUNK_SIZE = 64 * 1024;

    QFile file(filePath);
    if (!file.open(QIODevice::ExistingOnly | QIODevice::ReadOnly)) {
        logger.error() << "Failed to open:" << file.fileName() << file.errorString();
        return QByteArray();
    }
    qint64 fileSize = file.size();
    
    hmac_sha256_state sha;
    hmac_sha256_init(&sha, fileSize);

    std::array<char, FILE_CHUNK_SIZE> chunk;
    int chunk_len = 0;
    while (file.atEnd() == false)
    {
        chunk_len = file.read(chunk.data(), FILE_CHUNK_SIZE);
        hmac_sha256_update(&sha, chunk.data(), chunk_len);
    }

    hmac_sha256_finalize(&sha);

    return QByteArray(reinterpret_cast<const char*>(sha.hash.data()), sha.hash.size() * sizeof(uint32_t));
}


void HashManager::verifyHashes()
{
    QVector<QByteArray> hashes = HashConfigManager::getStoredHashes();

    for (int i = 0; i < m_files.size(); i++) {
        const QString file = m_files[i];
        QByteArray calculatedHash = calculateHash(file);
        if (calculatedHash != hashes[i]) {
            logger.critical() << "Found hash violation in file:" << file;
            emit violationDetected(eagle_eye::ViolationType::HashViolation);
            return;
        }
    }

    emit violationDetected(eagle_eye::ViolationType::NoViolation);
}

void HashManager::onSecurityCheck()
{
    verifyHashes();
}
