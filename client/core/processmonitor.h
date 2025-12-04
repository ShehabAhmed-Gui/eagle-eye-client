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

#ifndef PROCESSMONITOR_H
#define PROCESSMONITOR_H

#include <QString>
#include <QCoreApplication>

#include <string>
#include <vector>

#include "defs.h"
#include "process.h"

using eagle_eye::ViolationType;

class ProcessMonitor
{
public:
    explicit ProcessMonitor();

    ViolationType CheckForDllInjection();
    ViolationType CheckForDebugger();

private:
    struct ProcessInfo
    {
        // Absolute path to the executable
        std::wstring exePath;
        std::vector<std::wstring> startupModules;
    };

    // Info about the processes we monitor
    std::vector<ProcessInfo> processes;

    // Checks if the process is running
    // Updates ProcessInfo accordingly and returns a handle to the process
    Process::ProcessHandle lookForProcess(ProcessInfo& process);

    // Returns true if malicious DLL injection is detected
    bool checkDLLInjection(Process::ProcessHandle& processHandle, const ProcessInfo& processInfo);

    bool isModuleVerified(const ProcessInfo& process, const std::wstring modulePath);

};

#endif // PROCESSMONITOR_H
