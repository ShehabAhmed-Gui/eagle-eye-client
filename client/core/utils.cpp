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

#include "utils.h"
#include "logger.h"

namespace {
Logger logger("Utils");
}

Utils::Utils() {}

QString Utils::getOpenSSLPath()
{
#if defined(Q_OS_WIN)
    return QString(QCoreApplication::applicationDirPath() + "//openssl/openssl.exe");
#endif
}

QByteArray Utils::getSecureKey()
{
    QString opensslPath = getOpenSSLPath();
    QStringList args;
    args << "rand" << "-hex" << "32";
    QProcess process;

    process.setProgram(opensslPath);
    process.setArguments(args);
    process.start();

    bool ok = process.waitForFinished(3000);
    if (!ok) {
        logger.error() << "QProcess failed with error:" << process.errorString();
        return QByteArray();
    }

    QByteArray output = process.readAllStandardOutput();

    // TODO: remove this debug statement
    logger.debug() << "Generated secure key:" << output;

    return output;
}

QString Utils::getServiceLocation()
{
    return QCoreApplication::applicationDirPath();
}

QString Utils::getMainAppLocation()
{
    QDir dir(QCoreApplication::applicationDirPath());
    dir.cdUp();

    return dir.absolutePath();
}
