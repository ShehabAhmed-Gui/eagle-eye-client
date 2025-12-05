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

#include "logger.h"
#include "processmonitor.h"
#include "process.h"
#include "filesmanager.h"
#include "utils.h"

#include <vector>

namespace {
Logger logger("ProcessMonitor");
}

using eagle_eye::ViolationType;

namespace Mgmt = Process::Management;
namespace Sec = Process::Security;

ProcessMonitor::ProcessMonitor()
{
    QVector<QString> exePaths = FilesManager::getExeFiles(Utils::getMainAppLocation());

    for (const QString &exePath : std::as_const(exePaths))
    {
        ProcessInfo info;
        info.exePath = exePath.toStdWString();

        processes.push_back(info);

        logger.debug() << "Monitoring Exe:" << info.exePath;
    }
}

ViolationType ProcessMonitor::CheckForDllInjection()
{
    for (ProcessInfo& process : processes) {
        Process::ProcessHandle processHandle = Mgmt::getProcess(process.exePath);
        if (processHandle.isValid() == false) {
            continue;
        }
        logger.debug() << "Checking" << process.exePath
                       << "for dll injections";

        // Check for malicious DLL injection
        if (checkDLLInjection(processHandle, process)) {
            processHandle.close();
            return ViolationType::DLLInjectionViolation;
        }

        processHandle.close();
    }

    return ViolationType::NoViolation;
}

ViolationType ProcessMonitor::CheckForDebugger()
{
    // 1. Check if our service is running
    // in a debugger
    Process::ProcessHandle service_process = {};
    service_process.id = GetCurrentProcess();
    if (Sec::hasDebugger(service_process)) {
        return ViolationType::EagleEyeRunningInADebugger;
    }
    service_process.close();

    // 2. Go through every executable we monitor
    // check if its running as a process
    // and perform monitor checks on it
    for (ProcessInfo& process : processes) {
        Process::ProcessHandle processHandle = Mgmt::getProcess(process.exePath);
        if (processHandle.isValid() == false) {
            continue;
        }
        logger.debug() << "Checking" << process.exePath
                       << "if running in a debugger";
        // Check if the process is running
        // in a debugger
        if (Sec::hasDebugger(processHandle)) {
            processHandle.close();
            return ViolationType::DebuggerViolation;
        }
    }

    return ViolationType::NoViolation;
}

bool ProcessMonitor::checkDLLInjection(Process::ProcessHandle& processHandle, const ProcessInfo& processInfo)
{
    if (processHandle.isValid() == false) {
        return false;
    }

    if (Sec::scanForMappedModules(processHandle)) {
        return true;
    }

    std::vector<std::wstring> currentModules = std::vector<std::wstring>(Process::Info::getProcessModules(processHandle));
    for (const std::wstring &modulePath : currentModules) {
        if (isModuleVerified(processInfo, modulePath) == false) {
            return true;
        }
    }

    return false;
}

bool ProcessMonitor::isModuleVerified(const ProcessInfo& process, const std::wstring modulePath)
{
    QString path = QString::fromStdWString(modulePath);
    if (path.contains("Windows\\SYSTEM32") || path.contains("Windows\\SysWOW64")) {
        return true;
    }

    // Check if its the process executable
    if (process.exePath == modulePath) {
        return true;
    }

    // Check if it has a valid digital signature
    if (!Sec::isFileSigned(modulePath)) {
        logger.warning() << "Unsigned module file:" << modulePath;
    }

    logger.warning() << "Unverified DLL:" << modulePath;
    return false;
}

