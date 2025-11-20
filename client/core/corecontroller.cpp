#include "corecontroller.h"
#include "logger.h"

#include <windows.h>
#include <process.h>
#include <Tlhelp32.h>
#include <winbase.h>

namespace {
Logger logger("CoreController");
}

CoreController::CoreController(QObject *parent)
    : QObject{parent}
{
}

void CoreController::init()
{
    // Our app will be in a subfolder of the main folder
    QDir dir = QDir(QCoreApplication::applicationDirPath());
    dir.cdUp();
    m_originalPath = dir.path();
}

void CoreController::onViolationDetected()
{

}

bool CoreController::killProcess(const QString &fileName)
{
    // Take a snapshot of all processes in the system.
    HANDLE snapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
    PROCESSENTRY32 pe32;

    auto cleanup = qScopeGuard( [snapShot] {
        CloseHandle(snapShot);
    });

    if (snapShot == INVALID_HANDLE_VALUE) {
        logger.error() << "CreateToolhelp32Snapshot failed for process:" << fileName;
        return false;
    }

    // Set the size of the structure before using it.
    pe32.dwSize = sizeof(PROCESSENTRY32);

    // Retrieve information about the first process,
    // and exit if unsuccessful
    BOOL hRes = Process32First(snapShot, &pe32);
    if (!hRes) {
        logger.error() << "Process32First failed" << GetLastError();
        return false;
    }

    while (hRes) {
        if (wcscmp(pe32.szExeFile, fileName.toStdWString().c_str()) == 0) {
            // Create a handle to the process
            HANDLE hprocess = OpenProcess(PROCESS_TERMINATE, false, (DWORD)pe32.th32ProcessID);

            if (hprocess != NULL) {
                logger.debug() << "Terminating:" << fileName;

                BOOL terminated = TerminateProcess(hprocess, 9);
                if (!terminated) {
                    logger.error() << "TerminateProcess failed. Error:" << GetLastError();
                    return false;
                }
                logger.debug() << "Terminated process:" << fileName;
            } else {
                logger.error() << "Could not create a handle to the process";
                return false;
            }
        }
        hRes = Process32Next(snapShot, &pe32);
    }

    return true;
}
