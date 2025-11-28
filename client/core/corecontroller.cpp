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
        killProcess(file);
    }
}

//TODO() move this to the process module
bool CoreController::killProcess(const QString &fileName)
{
    logger.debug() << "Terminating:" << fileName;

    // Take a snapshot of all processes in the system.
    HANDLE snapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
    PROCESSENTRY32 pe32;

    auto cleanup = qScopeGuard( [snapShot] {
        CloseHandle(snapShot);
    });

    if (snapShot == INVALID_HANDLE_VALUE) {
        logger.error() << "CreateToolhelp32Snapshot failed for process:" << fileName;
        return false;
    }

    // Set the size of the structure before using it.
    pe32.dwSize = sizeof(PROCESSENTRY32);

    // Retrieve information about the first process,
    // and exit if unsuccessful
    BOOL hRes = Process32First(snapShot, &pe32);
    if (!hRes) {
        logger.error() << "Process32First failed" << GetLastError();
        return false;
    }

    QFileInfo fileExeName(fileName);
    while (hRes) {
        if (wcscmp(pe32.szExeFile, fileExeName.fileName().toStdWString().c_str()) == 0) {
            // Create a handle to the process
            HANDLE hprocess = OpenProcess(PROCESS_TERMINATE, false, (DWORD)pe32.th32ProcessID);

            if (hprocess != NULL) {
                BOOL terminated = TerminateProcess(hprocess, 9);
                if (!terminated) {
                    logger.error() << "TerminateProcess failed. Error:" << GetLastError();
                    return false;
                }
                logger.debug() << "Terminated process:" << fileName;
            } else {
                logger.error() << "Could not create a handle to the process";
                return false;
            }
        } else {
            logger.debug() << "Process is not running";
        }
        hRes = Process32Next(snapShot, &pe32);
    }

    return true;
}
