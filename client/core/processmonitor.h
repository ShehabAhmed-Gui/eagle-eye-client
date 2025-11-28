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

#ifndef PROCESSMONITOR_H
#define PROCESSMONITOR_H

#include <QString>
#include <QCoreApplication>

#include <string>
#include <vector>

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
    std::vector<std::wstring> processModules;

    void lookForProcess();
};

#endif // PROCESSMONITOR_H
