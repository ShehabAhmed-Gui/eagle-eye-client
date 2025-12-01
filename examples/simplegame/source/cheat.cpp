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

#if defined(_WIN32)
#include <windows.h>
#include <winsvc.h>
#endif

#include "cheat.h"
#include "vendor/json.hpp"

#include <iostream>
#include <cstdlib>

namespace EagleEye
{
    const std::string service_name = "EagleEye service";
    const std::string pipe_name = R"(\\.\pipe\eagleeye)";
    const std::string app_id = "com.test.SimpleGame";

    const int MAX_MSG_SIZE = 1024;

    Connection::Connection()
    {
#if defined(_WIN32)
        pipe = nullptr;
#endif
    }

    Connection::~Connection()
    {
#if defined(_WIN32)
        CloseHandle(pipe);
#endif
    }

    bool Connection::connect()
    {
#if defined(_WIN32)
        pipe = CreateFileA(pipe_name.data(),
                                GENERIC_ALL,
                                FILE_SHARE_READ | FILE_SHARE_WRITE,
                                nullptr,
                                OPEN_EXISTING,
                                FILE_ATTRIBUTE_NORMAL,
                                nullptr);
        if (pipe == INVALID_HANDLE_VALUE) {
/*            std::cout << "Connecting to EagleEye pipe failed with error: "
                      << GetLastError() << std::endl;*/
            return false;
        }

//        std::cout << "Connected to EagleEye's pipe" << std::endl;
        return true;
#endif
        return false;
    }

    bool Connection::is_empty() const
    {
#if defined(_WIN32)
        return (pipe == INVALID_HANDLE_VALUE) || (pipe == nullptr);
#endif
    }

    bool Connection::send_message(const char* msg, int msg_size) const
    {
        if (is_empty()) {
            return false;
        }

//        std::cout << "Sending message: " << msg << std::endl;

#if defined(_WIN32)
        // Send message
        DWORD bytes_written;

        int code = WriteFile(pipe,
                msg,
                msg_size,
                &bytes_written,
                nullptr);

        return code != 0;
#endif
        
        return false;
    }

    std::string Connection::read_message() const
    {
        if (is_message_pending() == false) {
            return {};
        }

#if defined(_WIN32)
        DWORD bytes_read;
        char msg[MAX_MSG_SIZE];
        if (ReadFile(pipe,
                 msg,
                 MAX_MSG_SIZE,
                 &bytes_read,
                 nullptr) == 0) {
//            std::cout << "ReadFile failed with error: "
//                      << GetLastError() << std::endl;
            return {};
        }

        for (int i = 0; i < MAX_MSG_SIZE; i++)
        {
            if (msg[i] == '\n') {
                msg[i] = '\0';
                break;
            }
        }

        return std::string(msg);
#endif
        return {};
    }

    bool Connection::is_message_pending() const
    {
        if (is_empty()) {
            return false;
        }

#if defined(_WIN32)
        DWORD msg_size;
        PeekNamedPipe(pipe, nullptr,
                      0, nullptr,
                      &msg_size,
                      nullptr);
        
        if (msg_size != 0) {
            return true;
        }
#endif
        return false;
    }

    bool Connection::send_token_request() const
    {
        using nlohmann::json;

        json msg;
        msg["cmd"] = "token_request";
        msg["app_id"] = app_id;
        std::string msg_str = msg.dump();
        msg_str.push_back('\n');

        return send_message(msg_str.data(), msg_str.size() + 1);
    }

    bool is_anticheat_running()
    {
#if defined(_WIN32)
        SC_HANDLE service_manager = OpenSCManagerA(nullptr, nullptr, SC_MANAGER_ENUMERATE_SERVICE);

        if (service_manager == nullptr) {
            std::cout << "OpenSCManager Failed with code: "
                      << GetLastError() << std::endl;
            return false;
        }

        DWORD bytes_needed = NULL;
        DWORD service_count;

        // First call to get the required size of the array
        EnumServicesStatusA(service_manager,
                        SERVICE_WIN32,
                        SERVICE_ACTIVE,
                        NULL,            
                        0,
                        &bytes_needed,
                        &service_count,
                        nullptr);

        ENUM_SERVICE_STATUSA* services = (ENUM_SERVICE_STATUSA*)calloc(1, bytes_needed);

        // Second call to get the services status
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
                if (services[i].ServiceStatus.dwCurrentState == SERVICE_STOPPED) {
                    CloseServiceHandle(service_manager);
                    free(services);
                    return false;
                }

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
