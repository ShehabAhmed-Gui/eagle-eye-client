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
    bool ProcessHandle::isValid()
    {
#if defined(_WIN32)
        // Only nullptr is considered empty, pseudo-handle -1 is valid
        return (id != nullptr) || (id != INVALID_HANDLE_VALUE);
#endif
        return false;
    }

    bool hasDebugger(ProcessHandle& process)
    {
        if (!process.isValid()) return false;

#if defined(_WIN32)
        BOOL result = FALSE;
        BOOL success;

        if (IsDebuggerPresent()) {
            return true;
        }

        if (process.isValid()) {
            // Checks if the process is attached to a debugger
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
        BOOL terminated = TerminateProcess(process.id, 9);
        if (!terminated) {
            logger.error() << "TerminateProcess failed. Error:" << GetLastError();
            return;
        }
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

        auto cleanup = qScopeGuard([&processSnapshot]{
            CloseHandle(processSnapshot);
        });

        PROCESSENTRY32 processEntry;
        processEntry.dwSize = sizeof(PROCESSENTRY32);

        QFileInfo fileInfo(QString::fromStdWString(exePath));
        if (Process32First(processSnapshot, &processEntry) == TRUE) { 
           while (Process32Next(processSnapshot, &processEntry) == TRUE) {
                if (wcscmp(processEntry.szExeFile, fileInfo.fileName().toStdWString().c_str()) == 0) {
                    // To terminate a process
                    // we need a handle with at least PROCESS_TERMINATE access rights
                    HANDLE processHandle = OpenProcess(PROCESS_TERMINATE | PROCESS_QUERY_LIMITED_INFORMATION,
                                                                FALSE,
                                                                processEntry.th32ProcessID);

                    ProcessHandle process;
                    process.id = processHandle;

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
            return {};
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

    bool killProcess(const QString &fileName)
    {
        Process::ProcessHandle process = Process::getProcess(fileName.toStdWString());
        if (!process.isValid()) {
            logger.warning() << "Process is not running";
            return false;
        }

        BOOL terminated = TerminateProcess(process.id, 9);

        if (!terminated) {
            logger.error() << "Failed to kill process:" << fileName
                           << "Error:" << GetLastError();
            return false;
        }

        logger.debug() << "Terminated process:" << fileName;
        return true;
    }
}
