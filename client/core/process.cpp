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
        return getProcess(exePath);
#endif

        return process;
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

    std::vector<std::wstring> getProcessModules(ProcessHandle& process)
    {
#if defined(_WIN32)

        // wait until the process is waiting for input
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
            //TODO(omar): Log
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
