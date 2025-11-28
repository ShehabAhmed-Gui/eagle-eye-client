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

#include "corecontroller.h"
#include "logger.h"

#include <windows.h>
#include <process.h>
#include <Tlhelp32.h>
#include <winbase.h>

#include "utils.h"

namespace {
Logger logger("CoreController");
}

CoreController::CoreController(QObject *parent)
    : QObject{parent}
{
}

void CoreController::init()
{
    m_hashManager.reset(new HashManager(this));

    m_securityMonitor.reset(new SecurityMonitor(m_hashManager, this));
    m_securityMonitor->activate();

    m_daemonServer = new DaemonLocalServer(m_securityMonitor, this);
    m_daemonServer->start();

    connect(m_daemonServer, &DaemonLocalServer::mainProcessNotConnected, this,
            &CoreController::onMainProcessNotConnected);
}

void CoreController::onMainProcessNotConnected()
{
    // Attempt to terminate every process of main app
    QVector<QString> files = FilesManager::getExeFiles(Utils::getMainAppLocation());
    for (const QString &file : std::as_const(files)) {
        Process::killProcess(file);
    }
}
