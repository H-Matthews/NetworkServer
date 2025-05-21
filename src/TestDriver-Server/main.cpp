#include <iostream>

#include "NetworkServer.h"

int main()
{
    // Configure to run loopback on server number 200
    NetworkServer myServer(SocketUtils::Domain::IPV4, SocketUtils::Protocol::TCP,
                           std::string("127.0.0.1"), std::string("8070"));

    // Creates a socket
    if(! myServer.initializeSocket())
        exit(1);

    myServer.configureSocket();

    if(! myServer.bindAndListen())
        exit(1);

    myServer.printServerStatus();

    // Start Server
    myServer.start();

    return 0;
}