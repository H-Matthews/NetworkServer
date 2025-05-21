#pragma once

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <string>

#include "NetworkTypes.h"

struct SocketInfo
{
    SocketUtils::Domain domain;
    SocketUtils::Protocol transportProtocol;
    int fileDescriptor;

    // Unix Specific Values
    int unixDomainValue;
    int unixProtocolValue;

    SocketInfo(SocketUtils::Domain domain,
               SocketUtils::Protocol protocol) :
        domain(domain),
        transportProtocol(protocol),
        fileDescriptor(-1)
    { }
};

struct ServerAddressInfo
{
    std::string port;
    std::string ipAddr;

    // Unix Structure
    struct sockaddr_in serverAddr;

    ServerAddressInfo(const std::string address, const std::string port) :
        port(port),
        ipAddr(address),
        serverAddr()
    { }
};

class NetworkServer
{
    public:
        NetworkServer(SocketUtils::Domain domain, SocketUtils::Protocol protocol,
                      const std::string ipAddr, const std::string portNumber);

        // Wrapper around the Unix socket function
        bool initializeSocket();

        // Sets up the socket information
        void configureSocket();

        // IMPORTANT: Even though UDP does NOT use Listen
        // This function still must be called to bind, but it does NOT invoke listen on the socket
        bool bindAndListen();

        void start();

        void printServerStatus();
    private:
        void startTCPServer();
        void startUDPServer();

        // Converts to the UNIX defined value
        int convertDomain(SocketUtils::Domain domain);
        int convertProtocol(SocketUtils::Protocol protocol);

        // Checks Errno 
        void handleError();

        void handleRequest(int clientSocketFD);

    private:
        ServerAddressInfo mServerAddrInfo;
        SocketInfo mSocket;
        SocketUtils::Status mStatus;
};
