#include "processmonitor.h"
#include "process.h"

ProcessMonitor::ProcessMonitor()
{}

ViolationType ProcessMonitor::run()
{
    Process::ProcessHandle process = {};

    // Run checks one by one
    if (Process::hasDebugger(process)) {
        return ViolationType::DebuggerViolation;
    }

    return ViolationType::NoViolation;
}

