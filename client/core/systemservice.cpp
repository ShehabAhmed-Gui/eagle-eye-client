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

#include "systemservice.h"
#include <Windows.h>
#include <sddl.h>

SystemService::SystemService(int argc, char **argv)
    : QtService<QCoreApplication>(argc, argv, "EagleEye service")
{
    setServiceDescription("EagleEye service");
    // TODO: uncomment before go live
    //setServiceFlags(ServiceFlags::enum_type::CannotBeStopped);
}

void SystemService::start()
{
    if (!updateServicePermissions()) {
        qCritical() << "Failed to update service permissions";
        return;
    }

    qDebug() << "Updated service permissions";
}

void SystemService::stop()
{
    qDebug() << "Service stopped";
}

bool SystemService::updateServicePermissions()
{
    // Change service permissions at runtime
    auto scManager = OpenSCManager(NULL,
                                   NULL,
                                   SC_MANAGER_ALL_ACCESS);

    std::wstring servicename = serviceName().toStdWString();

    SC_HANDLE shandle = OpenService(scManager,
                                    servicename.c_str(),
                                    READ_CONTROL | WRITE_DAC);

    if (!shandle) {
        qCritical() << "OpenService failed:" << GetLastError();
        CloseServiceHandle(scManager);
        return false;
    }

    PSECURITY_DESCRIPTOR sd = nullptr;
    BOOL ok = ConvertStringSecurityDescriptorToSecurityDescriptorA(
        "D:(A;;FA;;;SY)"
        "(A;;CCDCLCSWRPWPDTLOCRRC;;;BA)"
        "(A;;CCLCSWLOCRRC;;;IU)"
        "(A;;CCLCSWLOCRRC;;;SU)",
        SDDL_REVISION_1,
        &sd,
        nullptr
        );

    if (!ok) {
        qCritical() << "ConvertStringSecurityDescriptor failed:" << GetLastError();
        CloseServiceHandle(shandle);
        CloseServiceHandle(scManager);
        return false;
    }

    // Apply new permissions
    SetServiceObjectSecurity(shandle,
                           DACL_SECURITY_INFORMATION | SACL_SECURITY_INFORMATION,
                           sd);

    if (!SetServiceObjectSecurity(shandle,
                                  DACL_SECURITY_INFORMATION,
                                  sd)) {
        qCritical() << "SetServiceObjectSecurity failed with error:"
                    << GetLastError();
        return false;
    }

    return true;
}
