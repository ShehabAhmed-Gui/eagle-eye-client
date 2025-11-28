#if defined(_WIN32)
#include <windows.h>
#include <winsvc.h>
#endif

#include "cheat.h"

#include <iostream>
#include <vector>
#include <cstdlib>

const std::string service_name = "EagleEye";
const std::string pipe_name = R"(\\.\pipe\eagleeye)";

namespace EagleEye
{


    Connection::Connection()
    {
#if defined(_WIN32)
        pipe = INVALID_HANDLE_VALUE;
#endif
    }

    bool Connection::is_empty() const
    {
#if defined(_WIN32)
        return pipe == INVALID_HANDLE_VALUE;
#endif
    }

    Connection create_connection()
    {
        Connection connection = Connection();

#if defined(_WIN32)
        HANDLE pipe = CreateFileA(pipe_name.data(),
                                GENERIC_ALL,
                                FILE_SHARE_READ | FILE_SHARE_WRITE,
                                nullptr,
                                OPEN_EXISTING,
                                FILE_ATTRIBUTE_NORMAL,
                                nullptr);
        if (pipe == INVALID_HANDLE_VALUE) {
            std::cout << "EAGLE-EYE Connection: Opening pipe failed. Error: " << GetLastError() << std::endl;
        }
        else {
            connection.pipe = pipe;
        }
#endif

        return connection;
    }

    bool send_message(const Connection& connection, const char* msg, int msg_size)
    {
        if (connection.is_empty()) {
            return false;
        }

#if defined(_WIN32)
        //Send message
        DWORD bytes_written;

        int code = WriteFile(connection.pipe,
                msg,
                msg_size,
                &bytes_written,
                nullptr);

        return code != 0;
#endif
        
        return false;
    }

    bool send_token_request(const Connection& connection)
    {
        //Send message
        std::string msg = R"(
            {
            "cmd": "token_request",
            "app_id: "com.test.TestChamber
            }            
        )";

        return send_message(connection, msg.data(), msg.size() + 1);
    }

    bool is_anticheat_running()
    {
#if defined(_WIN32)
        SC_HANDLE service_manager = OpenSCManagerA(nullptr, nullptr, SC_MANAGER_ENUMERATE_SERVICE);

        if (service_manager == nullptr) {
            std::cout << "EAGLE-EYE: OpenSCManager Failed with code: " << GetLastError() << std::endl;
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
}
