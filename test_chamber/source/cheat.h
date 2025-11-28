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

        bool is_empty() const;
        bool send_token_request() const;
        bool send_message(const char* msg, int msg_size) const;

    };

    bool is_anticheat_running();
}

#endif
