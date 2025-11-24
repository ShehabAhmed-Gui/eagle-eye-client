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
#include "logger.h"

namespace {
Logger logger("Process");
}

namespace Process
{
    bool ProcessHandle::isEmpty()
    {
#if defined(_WIN32)
        return id == INVALID_HANDLE_VALUE;
#endif

        return false;
    }

    bool hasDebugger(ProcessHandle process)
    {
#if defined(_WIN32)
        BOOL result = FALSE;
        BOOL success;

        if (IsDebuggerPresent()) {
            return true;
        }

        if (!process.isEmpty()) {
            // Checks if the process running in a debugger
            success = CheckRemoteDebuggerPresent(process.id, &result);
        } else {
            logger.critical() << "Invalid process";
            return true;
        }
           
        return success && result;
#endif
        return false;
    }

    void closeProcess(ProcessHandle process)
    {
#if defined(_WIN32)
        TerminateProcess(process.id, 0);
#endif
    }

    ProcessHandle runProcess(std::wstring exePath)
    {
        ProcessHandle process = {};
#if defined(_WIN32)
        HINSTANCE success = ShellExecute(nullptr, nullptr,
                     exePath.data(),
                     nullptr,
                     nullptr,
                     SW_NORMAL);

        INT_PTR returnCode = (INT_PTR)success;
        if (returnCode <= 32) {
            logger.error() << "ShellExecute failed. Code:" << returnCode;
            return process;
        }

        // Get process id of the process we just ran
        HANDLE processSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        PROCESSENTRY32 processEntry;
        processEntry.dwSize = sizeof(PROCESSENTRY32);

        if (Process32First(processSnapshot, &processEntry) == TRUE)
        {
           while (Process32Next(processSnapshot, &processEntry) == TRUE)
           {
               std::wstring processExeFile = processEntry.szExeFile;
               if (processExeFile == exePath)
               {
                   process.id = OpenProcess(PROCESS_ALL_ACCESS,
                                                 FALSE,
                                                 processEntry.th32ProcessID);
               }
           }
        }
#endif
        return process;
    }

    std::wstring getProcessPath(ProcessHandle process)
    {
#if defined(_WIN32)
        wchar_t processPath[MAX_PATH];
        DWORD processPathSize = MAX_PATH;
        QueryFullProcessImageName(process.id, 0, processPath, &processPathSize);

        return std::wstring(processPath);
#endif
        return L"";
    }

    ProcessHandle getHandleToProcess(const QString &exePath)
    {
        ProcessHandle process = {};
        PROCESSENTRY32 entry;
        entry.dwSize = sizeof(PROCESSENTRY32);
        HANDLE processSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (processSnapshot == INVALID_HANDLE_VALUE) {
            logger.critical() << "Could not take a snapshot";
            return process;
        }

        if (Process32First(processSnapshot, &entry)) {
            do {
                if (QString::fromWCharArray(entry.szExeFile) == exePath) {
                    process.id = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);
                    break;
                }
            } while (Process32Next(processSnapshot, &entry));
        }

        return process;
    }
}
