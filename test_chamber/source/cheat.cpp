#if defined(_WIN32)
#include <windows.h>
#include <winsvc.h>
#endif

#include <iostream>
#include <vector>
#include <cstdlib>

const std::string service_name = "EagleEye";

bool is_anticheat_running()
{
#if defined(_WIN32)
    SC_HANDLE service_manager = OpenSCManagerA(nullptr, nullptr, SC_MANAGER_ENUMERATE_SERVICE);

    if (service_manager == nullptr) {
        std::cout << "OpenSCManager Failed with code: " << GetLastError() << std::endl;
        return false;
    }

    // TODO(omar): The SERVICE_ACTIVE flag includes paused/pause pending etc services
    // think about if this could be a problem for us

    DWORD bytes_needed;
    DWORD service_count;
    //First call to get the required size of the array 
    EnumServicesStatusA(service_manager,
                       SERVICE_WIN32,
                       SERVICE_ACTIVE,  
                       NULL,            
                       0,
                       &bytes_needed,
                       &service_count,
                       nullptr);

    ENUM_SERVICE_STATUSA* services = (ENUM_SERVICE_STATUSA*)calloc(1, bytes_needed);

    //Second call to get the services status
    EnumServicesStatusA(service_manager,
                       SERVICE_WIN32,
                       SERVICE_ACTIVE,  
                       services,            
                       bytes_needed,
                       &bytes_needed,
                       &service_count,
                       nullptr);

    for (int i = 0; i < service_count; i++)
    {
        if (service_name == services[i].lpServiceName) {
            CloseServiceHandle(service_manager);
            free(services);

            return true;
        }
    }

    CloseServiceHandle(service_manager);
    free(services);

    return false;

#endif

    return false;
}
