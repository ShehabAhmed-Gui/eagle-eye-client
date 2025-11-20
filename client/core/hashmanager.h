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
