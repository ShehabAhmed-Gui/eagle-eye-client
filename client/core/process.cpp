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

#if defined(_WIN32)
#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <psapi.h>
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

    void closeProcess(ProcessHandle& process)
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
        return getProcess(exePath);
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

    ProcessHandle getProcess(std::wstring exePath)
    {
#if defined(_WIN32)
        HANDLE processSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        PROCESSENTRY32 processEntry;
        processEntry.dwSize = sizeof(PROCESSENTRY32);

        if (Process32First(processSnapshot, &processEntry) == TRUE) { 
           while (Process32Next(processSnapshot, &processEntry) == TRUE)
           {
                HANDLE checkedProcessHandle = OpenProcess(PROCESS_QUERY_INFORMATION,
                                                            FALSE,
                                                            processEntry.th32ProcessID);
                if (checkedProcessHandle == nullptr) {
                    // Normal behavior, we just can't access that process
                    continue;
                }
                
                wchar_t fullPath[MAX_PATH];
                DWORD fullPathSize = MAX_PATH;

                if (QueryFullProcessImageName(checkedProcessHandle, 0, fullPath, &fullPathSize) == 0) {
                    //TODO(omar): Log
                    CloseHandle(checkedProcessHandle);
                    continue;
                }
                std::wstring processExeFile = fullPath;

                CloseHandle(checkedProcessHandle);

                if (processExeFile == exePath) {
                    ProcessHandle process;
                    process.id = OpenProcess(PROCESS_ALL_ACCESS,
                                                 FALSE,
                                                 processEntry.th32ProcessID);
                    return process;
               }
           }
        }
#endif
        return ProcessHandle();
    }

    ProcessHandle getProcessByExeName(std::wstring exePath)
    {
#if defined(_WIN32)
        HANDLE processSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        PROCESSENTRY32 processEntry;
        processEntry.dwSize = sizeof(PROCESSENTRY32);

        if (Process32First(processSnapshot, &processEntry) == TRUE) { 
           while (Process32Next(processSnapshot, &processEntry) == TRUE)
           {
                std::wstring processExeFile = processEntry.szExeFile;

                if (processExeFile == exePath) {
                    ProcessHandle process;
                    process.id = OpenProcess(PROCESS_ALL_ACCESS,
                                                 FALSE,
                                                 processEntry.th32ProcessID);
                    return process;
               }
           }
        }
#endif
        return ProcessHandle();
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
    std::vector<std::wstring> getProcessModules(ProcessHandle& process)
    {
#if defined(_WIN32)
        // Wait until the process is waiting for input
        // increasing the chance that all the "startup" dlls have been injected
        WaitForInputIdle(process.id, 2000);

        HMODULE modules[1024];
        DWORD bytesNeeded;
        if (EnumProcessModules(process.id, modules, sizeof(modules), &bytesNeeded) == 0) {
            //TODO(omar): Log
        }

        int moduleCount = bytesNeeded / sizeof(HMODULE);
        std::vector<std::wstring> modulePaths(moduleCount);

        for (int i = 0; i < moduleCount; i++)
        {
            TCHAR moduleName[MAX_PATH];

            // Get the full path to the module's file.
            if (GetModuleFileNameEx(process.id, modules[i], moduleName,
                                    sizeof(moduleName) / sizeof(TCHAR))) {
                modulePaths[i] = moduleName;
            }
        }

        return modulePaths;
#endif
        return {};
    }

    std::vector<uint64_t> getProcessThreads(ProcessHandle& process)
    {
#if defined(_WIN32)
        HANDLE threadSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
        THREADENTRY32 threadEntry;
        threadEntry.dwSize = sizeof(THREADENTRY32);

        if (Thread32First(threadSnapshot, &threadEntry) == FALSE) {
            // TODO(omar): Log
            CloseHandle(threadSnapshot);
            return {};
        }

        std::vector<uint64_t> threadIds;
        do
        {
            if (threadEntry.th32OwnerProcessID == GetProcessId(process.id)) {
                threadIds.push_back(threadEntry.th32ThreadID);
            }
        } while (Thread32Next(threadSnapshot, &threadEntry));

        CloseHandle(threadSnapshot);

        return threadIds;
#endif
        return {};
    }
}
