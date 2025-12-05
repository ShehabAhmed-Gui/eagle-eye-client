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

class ProcessMonitor
{
public:
    explicit ProcessMonitor();

    eagle_eye::ViolationType CheckForDllInjection();
    eagle_eye::ViolationType CheckForDebugger();

private:
    struct ProcessInfo
    {
        // Absolute path to the executable
        std::wstring exePath;
    };

    // Info about the processes we monitor
    std::vector<ProcessInfo> processes;

    // Returns true if malicious DLL injection is detected
    bool checkDLLInjection(Process::ProcessHandle& processHandle, const ProcessInfo& processInfo);

    bool isModuleVerified(const ProcessInfo& process, const std::wstring modulePath);
};

#endif // PROCESSMONITOR_H
