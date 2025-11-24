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

#ifndef HASHMANAGER_H
#define HASHMANAGER_H

#include <QObject>
#include <QByteArray>
#include <QDebug>
#include <QCoreApplication>

#include "defs.h"
#include "keychainmanager.h"

class HashManager : public QObject
{
    Q_OBJECT

public:
    explicit HashManager(const QString originalPath,
                         QSharedPointer<KeychainManager> keychainManger,
                         QObject *parent = nullptr);

    void activate();

public slots:
    void verifyHashes();

signals:
    void violationDetected(eagle_eye::ViolationType type);

public slots:
    void onReadyRead(const QString &data);
    void onSecurityCheck();

private:
    QByteArray calculateHash(const QString &filePath);
    void retrieveStoredHashes();

    QSharedPointer<KeychainManager> m_keychainManger;

    QString m_key;
    QString m_originalPath;
    QVector<QString> m_files;
    QVector<QByteArray> m_storedHashes;
    int m_pendingHashes = 0;
};

#endif // HASHMANAGER_H
