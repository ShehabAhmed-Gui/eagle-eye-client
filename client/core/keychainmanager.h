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
