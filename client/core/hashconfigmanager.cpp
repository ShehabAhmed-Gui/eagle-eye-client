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

#include "hashconfigmanager.h"
#include "logger.h"
#include "utils.h"

namespace {
Logger logger("HashConfigManager");
}

HashConfigManager::HashConfigManager()
{
}

void HashConfigManager::storeHashes(const QVector<QByteArray> &hashes)
{
    // If we let Qt resolve the current app dir for the file
    // it will use C:\Windows\System32 since services inherit
    // the working directory from SCM
    const QString appDir = Utils::getAppPath();
    const QString storageFilePath = QDir(appDir).filePath("storage.json");

    QFile configFile(storageFilePath);
    if (!configFile.open(QIODevice::WriteOnly)) {
        logger.error() << "Could not open config file. Error:" << configFile.errorString();
        return;
    }

    // Delete old configs
    configFile.resize(0);

    QJsonObject obj;
    QJsonArray hashesArr;

    for (const QByteArray &hash : hashes) {
        QJsonValue hashValue(QString::fromLatin1(hash.toBase64()));
        hashesArr.append(hashValue);
    }

    obj.insert("hashes", hashesArr);

    QJsonDocument doc(obj);
    configFile.write(doc.toJson(QJsonDocument::Indented));
    configFile.flush();
}

QVector<QByteArray> HashConfigManager::getStoredHashes()
{
    const QString appDir = Utils::getAppPath();
    const QString storageFilePath = QDir(appDir).filePath("storage.json");

    QFile configFile(storageFilePath);
    if (!configFile.open(QIODevice::ReadOnly)) {
        logger.error() << "Could not open config file. Error:" << configFile.errorString();
        return QVector<QByteArray>();
    }

    QJsonDocument doc = QJsonDocument::fromJson(configFile.readAll());
    QJsonObject obj = doc.object();

    // Read stored hashes
    QJsonArray arr = obj.value("hashes").toArray();

    QVector<QByteArray> result;

    for (QJsonValue value : arr) {
        const QByteArray base64 = value.toString().toUtf8();
        result.push_back(QByteArray::fromBase64(base64));
    }

    return result;
}
