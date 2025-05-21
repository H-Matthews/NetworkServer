#include "NetworkTypes.h"


std::string SocketUtils::toString(SocketUtils::Domain domain)
{
    std::string retString;

    switch(domain)
    {
        case SocketUtils::Domain::IPV4:
        {
            retString = "IPV4";
            break;
        }
        case SocketUtils::Domain::IPV6:
        {
            retString = "IPV6";
            break;
        }
        default:
        {
            retString = "VALUE_NOT_FOUND";
        }
    }

    return retString;
}

std::string SocketUtils::toString(SocketUtils::Protocol protocol)
{
    std::string retString;

    switch(protocol)
    {
        case SocketUtils::Protocol::UDP:
        {
            retString = "UDP";
            break;
        }
        case SocketUtils::Protocol::TCP:
        {
            retString = "TCP";
            break;
        }
        default:
        {
            retString = "VALUE_NOT_FOUND";
        }
    }

    return retString;
}

std::string SocketUtils::toString(SocketUtils::Status status)
{
    std::string retString;

    switch(status)
    {
        case SocketUtils::Status::UNINITIALIZED:
        {
            retString = "UNINITIALIZED";
            break;
        }
        case SocketUtils::Status::INITIALIZED:
        {
            retString = "INITIALIZED";
            break;
        }
        case SocketUtils::Status::READY:
        {
            retString = "READY";
            break;
        }
        case SocketUtils::Status::RUNNING:
        {
            retString = "RUNNING";
            break;
        }
        default:
        {
            retString = "VALUE_NOT_FOUND";
        }
    }

    return retString;
}