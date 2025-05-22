#include <iostream>

#include "NetworkServer.h"

int main()
{
    NetworkServer myServer(SocketUtils::Domain::IPV4, SocketUtils::Protocol::TCP,
                           std::string("8070"));

    // Creates a socket
    if(! myServer.initializeSocket())
        exit(1);

    myServer.printServerStatus();

    // Start Server
    myServer.start();

    return 0;
}