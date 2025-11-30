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

using namespace Process;

ProcessMonitor::ProcessMonitor()
{
    QVector<QString> exePaths = FilesManager::getExeFiles(Utils::getMainAppLocation());

    for (QString exePath : std::as_const(exePaths))
    {
        ProcessInfo info;
        info.exePath = exePath.toStdWString();

        processes.push_back(info);

        logger.debug() << "Monitoring Exe: " << info.exePath;
    }
}

ProcessHandle ProcessMonitor::lookForProcess(ProcessInfo& info)
{
    ProcessHandle processHandle = Process::getProcess(info.exePath);

    if (processHandle.isValid()) {
        // Make sure to not overwrite the startup modules
        if (info.startupModules.empty()) {
            info.startupModules = std::vector<std::wstring>(getProcessModules(processHandle));
        }
    }
    else {
        info.startupModules.clear();
    }

    return processHandle;
}

bool ProcessMonitor::isModuleVerified(const ProcessInfo& process, const std::wstring modulePath)
{
    // Check if its the process executable
    if (process.exePath == modulePath) {
        return true;
    }

    // Check if its part of the startup modules, which we deem as verified
    for (const std::wstring &startupModule : process.startupModules)
    {
        if (modulePath == startupModule) {
            return true;
        }
    }

    // Check if it has a valid digital signature
    if (isFileSigned(modulePath)) {
        return true;
    }

    logger.warning() << "Unverified DLL: " << modulePath;
    return false;
}

ViolationType ProcessMonitor::run()
{
    // 1. Check if our service is running
    // in a debugger
    ProcessHandle service_process = {};
    service_process.id = GetCurrentProcess();
    if (hasDebugger(service_process)) {
        return ViolationType::EagleEyeRunningInADebugger;
    }
    service_process.close();

    // 2. Go through every executable we monitor
    // check if its running as a process
    // and perform monitor checks on it
    for (ProcessInfo& process : processes) {
        ProcessHandle processHandle = lookForProcess(process);
        if (processHandle.isValid() == false) {
            continue;
        }

        // Check if the process is running
        // in a debugger
        if (hasDebugger(processHandle)) {
            processHandle.close();
            return ViolationType::DebuggerViolation;
        }

        // Check for malicious DLL injection
        std::vector<std::wstring> currentModules = std::vector<std::wstring>(getProcessModules(processHandle));
        for (const std::wstring &modulePath : currentModules) {
            if (isModuleVerified(process, modulePath) == false) {
                processHandle.close();
                return ViolationType::DLLInjectionViolation;
            }
        }

        processHandle.close();
    }

    return ViolationType::NoViolation;
}

