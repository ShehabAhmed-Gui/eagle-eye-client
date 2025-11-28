#ifndef CHEAT_H
#define CHEAT_H

namespace EagleEye
{
    struct Connection
    {
#if defined(_WIN32)
        typedef void* HANDLE;
        HANDLE pipe;
#endif

        Connection();
        bool is_empty() const;
    };

    Connection create_connection();
    void send_token_request(const Connection& connection);

    bool is_anticheat_running();
}

#endif
