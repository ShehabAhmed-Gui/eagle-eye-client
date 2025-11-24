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

#include "processmonitor.h"
#include "process.h"
#include "filesmanager.h"
#include "utils.h"

ProcessMonitor::ProcessMonitor()
{}

ViolationType ProcessMonitor::run()
{
    // First, check if EagleEye is running
    // in a debugger
    // Process::ProcessHandle process = {};
    // process.id = GetCurrentProcess();
    // if (Process::hasDebugger(process)) {
    //     return ViolationType::EaglEyeRunningInADebugger;
    // }

    // // Second, check if any process
    // // of the the main app is running in a debugger
    // for (const QString &file : FilesManager::getExeFiles(Utils::getAppPath())) {
    //     // Get a handle to the process
    //     process = Process::getHandleToProcess(file);
    //     if (Process::hasDebugger(process)) {
    //         return ViolationType::DebuggerViolation;
    //     }
    // }

    return ViolationType::NoViolation;
}

