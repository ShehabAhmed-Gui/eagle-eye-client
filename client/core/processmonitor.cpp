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

namespace {
Logger logger("ProcessMonitor");
}

ProcessMonitor::ProcessMonitor()
{
    lookForProcess();
}

void ProcessMonitor::lookForProcess()
{
    process = Process::getProcessByExeName(L"TestChamber.exe");

    if (process.isValid() == false) {
        //logger.error() << "Couldn't find the test chamber process";
    }
    else {
        logger.info() << "Test chamber process found";
        processModules = Process::getProcessModules(process);
    }
}

bool ProcessMonitor::isModuleVerified(const std::wstring modulePath)
{
    // Check if its the process executable
    std::wstring processPath = Process::getProcessPath(process);
    if (modulePath == processPath) {
        return true;
    }

    // Check if its part of the startup modules, which we deem as verified
    for (std::wstring startupModule : processModules)
    {
        if (modulePath == startupModule) {
            return true;
        }
    }

    // Check if it has a valid digital signature
    if (Process::isFileSigned(modulePath)) {
        return true;
    }

    logger.warning() << "Unverified DLL: " << modulePath;
    return false;
}

ViolationType ProcessMonitor::run()
{
    //Look for the process if we haven't found it yet
    if (process.isValid() == false) {
        lookForProcess();
    }

    // 1. Check if our service is running
    // in a debugger
    Process::ProcessHandle service_process = {};
    service_process.id = GetCurrentProcess();
    if (Process::hasDebugger(service_process)) {
        return ViolationType::EagleEyeRunningInADebugger;
    }

    // 2. Check if any process
    // of the the main app is running in a debugger
    for (const QString &file : FilesManager::getExeFiles(Utils::getMainAppLocation())) {
        // Get a handle to the process
        Process::ProcessHandle process = Process::getProcess(file.toStdWString());
        if (Process::hasDebugger(process)) {
            return ViolationType::DebuggerViolation;
        }
    }

    if (process.isValid()) {
        // 3. Check for injected dlls in the main executable

        // Check for unverified DLLs
        std::vector<std::wstring> currentModules = Process::getProcessModules(process);

        for (std::wstring modulePath : currentModules)
        {
            if (isModuleVerified(modulePath) == false) {
                return ViolationType::DLLInjectionViolation;
            }
        }
    }

    return ViolationType::NoViolation;
}

