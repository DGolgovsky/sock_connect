#ifndef UNIX_SOCKET_H
#define UNIX_SOCKET_H

#include "Connection.h"

#include <sys/un.h>

class UNIX : public Connection
{
private:
    sockaddr_un socket;
public:
    UNIX(uint32_t &path);

    ~UNIX() override;
};

#endif // UNIX_SOCKET_H
