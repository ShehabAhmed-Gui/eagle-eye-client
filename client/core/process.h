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

#ifndef PROCESS_H
#define PROCESS_H

#include <QString>

#ifdef _WIN32
#include <windows.h>
#include <tlhelp32.h>
#endif

#include <string>
#include <vector>

namespace Process
{
    // ProcessHandle's id is by default set to whatever the invalid value is
    // for the platform. for example win32 is INVALID_HANDLE_VALUE
    // linux is -1 (pid_t)
    // this signals that the process handle is empty/no process has been specified
    struct ProcessHandle
    {
#if defined(_WIN32)
        HANDLE id = INVALID_HANDLE_VALUE;
#endif
        // Checks if the process id/handle is invalid
        bool isValid();
    };

    void closeProcess(ProcessHandle& process);
    bool killProcess(const QString &fileName);

    // Runs the specified executable (absolute path), returns the handle to its process
    ProcessHandle runProcess(std::wstring exePath);

    // Gets a current running process by its executable path(absolute path)
    ProcessHandle getProcess(std::wstring exePath);

    // Gets a current running process by its executable name
    ProcessHandle getProcessByExeName(std::wstring exePath);

    // Returns the absolute path of the process executable
    std::wstring getProcessPath(ProcessHandle& process);

    // Get paths of all the modules attached to the process.
    // on windows, this includes the main EXE and all loaded DLLs,
    std::vector<std::wstring> getProcessModules(ProcessHandle& process);

    // Get ids of all threads owned by the process
    std::vector<uint64_t> getProcessThreads(ProcessHandle& process);

    bool hasDebugger(ProcessHandle& process);
}

#endif
