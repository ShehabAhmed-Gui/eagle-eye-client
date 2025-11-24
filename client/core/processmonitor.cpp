#include "logger.h"
#include "processmonitor.h"
#include "process.h"


namespace {
Logger logger("ProcessMonitor");
}

ProcessMonitor::ProcessMonitor()
{
    process = Process::getProcessByExeName(L"TestChamber.exe");

    if (process.isEmpty()) {
        logger.error() << "Couldn't find the test chamber process";
    }
    else {
        logger.info() << "Test chamber process found";
        processModules = Process::getProcessModules(process);
    }
}

ViolationType ProcessMonitor::run()
{
    if (process.isEmpty()) {
        return ViolationType::NoViolation;
    }

    // Run checks one by one
    if (Process::hasDebugger(process)) {
        return ViolationType::DebuggerViolation;
    }

    std::vector<std::wstring> currentModules = Process::getProcessModules(process);
    if (currentModules != processModules)
    {
        return ViolationType::DLLInjectionViolation;
    }

    return ViolationType::NoViolation;
}

