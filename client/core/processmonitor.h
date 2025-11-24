#ifndef PROCESSMONITOR_H
#define PROCESSMONITOR_H

#include <QString>
#include <QCoreApplication>

#include "defs.h"
#include "process.h"

using ViolationType = eagle_eye::ViolationType;

class ProcessMonitor
{
public:
    explicit ProcessMonitor();

    // Returns the violation type
    ViolationType run();

private:
    Process::ProcessHandle process;
};

#endif // PROCESSMONITOR_H
