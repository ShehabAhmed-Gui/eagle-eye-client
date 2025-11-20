#include "hashmanager.h"
#include "hash.h"
#include "filesmanager.h"
#include "logger.h"

namespace {
Logger logger("HashManager");
}

HashManager::HashManager(const QString originalPath,
                         QObject *parent)
    : QObject{parent}
    , m_originalPath(originalPath)
{
}

void HashManager::activate()
{
    QVector<QString> files = FilesManager::getProgramFiles(m_originalPath);
    m_files = files;

    // Calculate and store file hash
    for (QString &file : files) {
        QByteArray hash = calculateHash(file);
        if (hash.isNull()) {
            logger.error() << "Could not calculate hash for:" << file;
            return;
        }

        m_hashes[file] = hash;
    }

    logger.debug() << "Calculated hashes for" << m_files.count() << "files";
}

/*
* We calculate the hash by streaming the file chunk by chunk to not hog memory
* Chunk size is arbitrary, can be anything
*/
QByteArray HashManager::calculateHash(const QString &filePath)
{
    const int FILE_CHUNK_SIZE = 1000000; //Maximum size of a chunk in bytes

    QFile file(filePath);
    if (!file.open(QIODevice::ExistingOnly | QIODevice::ReadOnly)) {
        logger.error() << "Failed to open:" << file.fileName() << file.errorString();
        return QByteArray();
    }
    qint64 fileSize = file.size();
    
    sha256_state sha;
    sha256_init(&sha, fileSize);

    QDataStream dataStream(&file);
    std::array<char, FILE_CHUNK_SIZE> chunk;
    int chunk_len = 0;
    
    while (dataStream.atEnd() == false)
    {
        chunk_len = dataStream.readRawData(chunk.data(), FILE_CHUNK_SIZE);
        sha256_update(&sha, chunk.data(), chunk_len);
    }

    sha256_finalize(&sha);

    return QByteArray(reinterpret_cast<const char*>(sha.hash.data()), sha.hash.size() * sizeof(uint32_t));
}

void HashManager::verifyHashes()
{
    for (QString &file : m_files) {
        if (m_hashes.find(file) == m_hashes.end()) {
            logger.error() << "Could not find an stored hash for:" << file;
            return;
        }

        if (calculateHash(file) != m_hashes[file]) {
            logger.warning() << "Found a violation in file:" << file;
            emit violationDetected(ViolationType::HashViolation);
            return;
        }
    }
}
