#pragma once

#include <string>

namespace SocketUtils
{
    enum class Domain
    {
        IPV4 = 0,
        IPV6
    };

    std::string toString(SocketUtils::Domain domain);

    enum class Protocol
    {
        TCP = 0,
        UDP
    };

    std::string toString(SocketUtils::Protocol protocol);

    enum class Status
    {
        UNINITIALIZED = 0,
        INITIALIZED,
        READY,
        RUNNING
    };

    std::string toString(SocketUtils::Status status);
}
