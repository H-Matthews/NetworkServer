#include "NetworkServer.h"

#include <iostream>

#include <errno.h>
#include <string.h>
#include <stdio.h>

NetworkServer::NetworkServer(SocketUtils::Domain domain, SocketUtils::Protocol protocol,
                             const std::string ipAddr, const std::string portNumber ) :
    mServerAddrInfo(ipAddr, portNumber),
    mSocket(domain, protocol),
    mStatus(SocketUtils::Status::UNINITIALIZED)
{
}

bool NetworkServer::initializeSocket()
{
    // DOMAIN (IPV4, IPV6)
    int socketDomain = convertDomain(mSocket.domain);
    if(socketDomain == -1)
        return false;

    // TRANSPORT PROTOCOL (TCP, UDP)
    int socketProtocol = convertProtocol(mSocket.transportProtocol);
    if(socketProtocol == -1)
        return false;

    // Request a socket endpoint
    mSocket.fileDescriptor = socket(socketDomain, socketProtocol, 0);
    if(mSocket.fileDescriptor < 0)
    {
        handleError();
        std::cout << "Failed to create Socket! " << std::endl;
        return false;
    }

    mStatus = SocketUtils::Status::INITIALIZED;

    return true;
}

void NetworkServer::configureSocket()
{
    mServerAddrInfo.serverAddr.sin_family = mSocket.unixDomainValue;

    // Convert to Network Byte Order (Big Endian)
    mServerAddrInfo.serverAddr.sin_port = htons( std::stoi( mServerAddrInfo.port ) );
    mServerAddrInfo.serverAddr.sin_addr.s_addr = inet_addr( mServerAddrInfo.ipAddr.c_str() );

    return;
}

bool NetworkServer::bindAndListen()
{
    if( bind( mSocket.fileDescriptor, (struct sockaddr*)&mServerAddrInfo.serverAddr, sizeof(mServerAddrInfo.serverAddr)) < 0)
    {
        handleError();
        std::cout << "Failed to bind Socket! " << std::endl;
        return false;
    }

    // Listen is only for TCP
    if(! (mSocket.transportProtocol == SocketUtils::Protocol::UDP) )
    {
        if( listen( mSocket.fileDescriptor, 10) < 0)
        {
            handleError();
            std::cout << "Invoking listen on the binded socket failed " << std::endl;
            return false;
        }
    }

    mStatus = SocketUtils::Status::READY;

    return true;
}

void NetworkServer::start()
{
    if(mStatus != SocketUtils::Status::READY)
    {
        std::cout << "The server was NOT in a READY state to start... " << std::endl;
        return;
    }

    if(mSocket.transportProtocol == SocketUtils::Protocol::TCP)
    {
        startTCPServer();
    }
    else if(mSocket.transportProtocol == SocketUtils::Protocol::UDP)
    {
        startUDPServer();
    }

    // Close Socket
    close(mSocket.fileDescriptor);

    return;
}

void NetworkServer::startTCPServer()
{
    // TODO make this more robust and apart of the class
    struct sockaddr_in clientAddr;
    socklen_t clientAddrSize = sizeof(struct sockaddr_in);

    std::cout << "Server is RUNNING. Awaiting TCP Connections..." << std::endl;
    
    mStatus = SocketUtils::Status::RUNNING;
    while( 1 )
    {
        // This call is blocking, it sits and waits for client connections
        int clientSocketFD = accept(mSocket.fileDescriptor, (struct sockaddr*)&clientAddr, &clientAddrSize);
        if(clientSocketFD < 0 )
        {
            handleError();
            std::cout << "Failed to accept client request " << std::endl;
        }

        std::cout << "Received a connection! " << std::endl;

        handleRequest(clientSocketFD);
        std::cout << "Client Dealt with" << std::endl;
    }

    mStatus = SocketUtils::Status::READY;
    return;
}

void NetworkServer::startUDPServer()
{
    struct sockaddr_in clientAddr;
    socklen_t clientAddrSize = sizeof(struct sockaddr_in);

    char buffer[100];
    char message[] = "Hello Client";

    std::cout << "Server is RUNNING. Awaiting UDP Messages..." << std::endl;

    mStatus = SocketUtils::Status::RUNNING;
    while( 1 )
    {
        int n = recvfrom(mSocket.fileDescriptor, buffer, sizeof(buffer), 0, 
            (struct sockaddr*)&clientAddr, &clientAddrSize);
    
        // Print client message
        buffer[n] = '\0';
        puts(buffer);

        // TODO Handle Request and send Response
    }

    mStatus = SocketUtils::Status::READY;
    return;
}

void NetworkServer::handleRequest(int clientSocketFD)
{
    char rBuffer[100] = "";
    char sBuffer[100] = "";
    int n;

    while(1)
    {
        recv(clientSocketFD, rBuffer, sizeof(rBuffer), 0);
    
        printf("\n[client] %s", rBuffer);
        if(strcmp(rBuffer, "exit" ) == 0)
            break;

        printf("\n[server] " );
        n = 0;
        while( (sBuffer[n++] = getchar() != '\n') );
        send(clientSocketFD, sBuffer, sizeof(sBuffer), 0);
        if(strcmp(sBuffer, "exit") == 0)
            break;
        printf("\n");
    }

    return;
}

// UTILITY FUNCTIONS BELOW ------------------------------------

// Converts to the UNIX defined value
int NetworkServer::convertDomain(SocketUtils::Domain domain)
{
    int unixValue = 0;

    switch(domain)
    {
        case SocketUtils::Domain::IPV4:
        {
            unixValue = AF_INET;
            break;
        }
        case SocketUtils::Domain::IPV6:
        {
            unixValue = AF_INET6;
            break;
        }
        default:
        {
            std::cout << "Socket Domain ERROR! The domain given is NOT currently being supported " << std::endl;
            unixValue = -1;
        }
    }

    mSocket.unixDomainValue = unixValue;

    return unixValue;
}

int NetworkServer::convertProtocol(SocketUtils::Protocol protocol)
{
    int unixProtocol = 0;

    switch(protocol)
    {
        case SocketUtils::Protocol::TCP:
        {
            unixProtocol = __socket_type::SOCK_STREAM;
            break;
        }
        case SocketUtils::Protocol::UDP:
        {
            unixProtocol = __socket_type::SOCK_DGRAM;
            break;
        }
        default:
        {
            std::cout << "Socket Protocol ERROR! The protocol given is NOT currently being supported by this program " 
                      << std::endl;
            unixProtocol = -1;
        }
    }

    mSocket.unixProtocolValue = unixProtocol;

    return unixProtocol;
}

// Prints Error code via perror
void NetworkServer::handleError()
{
    perror("Error Code: ");

    return;
}

void NetworkServer::printServerStatus()
{
    // Print Server IP / PORT Info
    std::cout << "\nNETWORK SERVER ---------------------------------------------\n";
    std::cout << "\t STATUS            : " << SocketUtils::toString(mStatus) << "\n";
    std::cout << "\t Listening Address : " << mServerAddrInfo.ipAddr << "\n";
    std::cout << "\t Listening Port    : " << mServerAddrInfo.port << "\n\n";

    std::cout << "\t Socket Domain     : " << SocketUtils::toString(mSocket.domain) << "\n";
    std::cout << "\t\t Unix Domain value   : " << mSocket.unixDomainValue << "\n";
    std::cout << "\t Socket Protocol   : " << SocketUtils::toString(mSocket.transportProtocol) << "\n";
    std::cout << "\t\t Unix Protocol value : " << mSocket.unixProtocolValue << "\n";
    std::cout << "\t Socket FD         : " << mSocket.fileDescriptor << "\n" << std::endl;

    return;
}
