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

#include "process.h"

#if defined(_WIN32)
#include <windows.h>
#include <tlhelp32.h>
#endif

namespace Process
{
    bool ProcessHandle::isEmpty()
    {
#if defined(_WIN32)

        return id == INVALID_HANDLE_VALUE;

#endif

        return false;
    }

    bool hasDebugger(ProcessHandle& process)
    {
#if defined(_WIN32)

        BOOL result = FALSE;
        BOOL success;

        if (process.isEmpty()) {
            // Check the current process

            // First check if the process is running in the context of a
            // user mode debugger
            // Unlike CheckRemoteDebuggerPresent which checks for a debugger
            // as a seperate (parallel) process.
            // TODO(): this check might be redundant
            if (IsDebuggerPresent())
            {
                return true;
            }

            // Get the current process if no process was specified
            success = CheckRemoteDebuggerPresent(GetCurrentProcess(), &result);
        }
        else { 
            success = CheckRemoteDebuggerPresent(process.id, &result);
        }
           
        return success && result;

#endif

        return false;
    }

    void closeProcess(ProcessHandle& process)
    {
#if defined(_WIN32)

        TerminateProcess(process.id, 0);

#endif
    }

    //TODO(omar): (WIN32) use a call other than ShellExecute, so that this works in the service
    ProcessHandle runProcess(std::wstring exePath)
    {
        ProcessHandle process = {};
#if defined(_WIN32)

        HINSTANCE returnCode = ShellExecute(nullptr, nullptr,
                     exePath.data(),
                     nullptr,
                     nullptr,
                     SW_NORMAL);

        // TODO(omar): check return code

        // Get process id of the process we just ran
        HANDLE processSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        PROCESSENTRY32 processEntry;
        processEntry.dwSize = sizeof(PROCESSENTRY32);

        if (Process32First(processSnapshot, &processEntry) == TRUE) { 
           while (Process32Next(processSnapshot, &processEntry) == TRUE)
           {
               std::wstring processExeFile = processEntry.szExeFile;
               if (processExeFile == exePath) {
                   process.id = OpenProcess(PROCESS_ALL_ACCESS,
                                                 FALSE,
                                                 processEntry.th32ProcessID);
               }
           }
        }

#endif

        return process;
    }


    std::wstring getProcessPath(ProcessHandle& process)
    {
#if defined(_WIN32)
        wchar_t processPath[MAX_PATH];
        DWORD processPathSize = MAX_PATH;
        QueryFullProcessImageName(process.id, 0, processPath, &processPathSize);

        return std::wstring(processPath);

#endif

        return L"";
    }


}
