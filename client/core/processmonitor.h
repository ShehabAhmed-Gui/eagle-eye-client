#ifndef PROCESSMONITOR_H
#define PROCESSMONITOR_H

#include <QString>
#include <QCoreApplication>

#include <Windows.h>
#include "defs.h"

using ViolationType = eagle_eye::ViolationType;

class ProcessMonitor
{
public:
    explicit ProcessMonitor();

    // Returns the violation type
    static ViolationType run();
};

#endif // PROCESSMONITOR_H
