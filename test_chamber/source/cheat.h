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

#ifndef CHEAT_H
#define CHEAT_H

namespace EagleEye
{
    struct Connection
    {

private:
    #if defined(_WIN32)
        typedef void* HANDLE;
        HANDLE pipe;
    #endif

public:
        Connection();
        ~Connection();

        bool connect();
        bool is_empty() const;
        bool send_token_request() const;

        bool send_message(const char* msg, int msg_size) const;

        // returns true if there is a message waiting to be read by us
        bool Connection::is_message_pending() const;
    };

    bool is_anticheat_running();
}

#endif
