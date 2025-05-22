#include "NetworkServer.h"

#include <iostream>

#include <errno.h>
#include <string.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

NetworkServer::NetworkServer(SocketUtils::Domain domain, SocketUtils::Protocol protocol,
                             const std::string portNumber ) :
    mServerAddrInfo(portNumber),
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

    bool status = false;
    if(mSocket.transportProtocol == SocketUtils::Protocol::TCP)
    {
        status = initializeTCPSocket();
    }
    else if( mSocket.transportProtocol == SocketUtils::Protocol::UDP)
    {
        status = initializeUDPSocket();
    }

    return status;
}

bool NetworkServer::initializeTCPSocket()
{
    struct addrinfo hints;
    struct addrinfo* serverInfo;
    memset( &hints, 0, sizeof(hints) );

    // Fill out hints 
    hints.ai_family = mSocket.unixDomainValue;
    hints.ai_socktype = mSocket.unixProtocolValue;
    hints.ai_flags = AI_PASSIVE;    // Find a suitable IP

    // returns 0 if it succeeds
    int retStatus = getaddrinfo(NULL, mServerAddrInfo.port.c_str(), &hints, &serverInfo );
    if( retStatus != 0)
    {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror( retStatus ) );
        return false;
    }

    // Walk through our serverInfo list, and attempt to bind to the first possible address

    struct addrinfo* current = serverInfo;
    char ipstr[INET6_ADDRSTRLEN];
    int yes = 1;
    while( current != nullptr)
    {
        mSocket.fileDescriptor = socket(current->ai_family, current->ai_socktype, current->ai_protocol );
        if(mSocket.fileDescriptor == -1)
        {
            perror("Server: Socket");
            continue;
        }

        // Prevents the error message "Address in use"
        if(setsockopt(mSocket.fileDescriptor, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        {
            perror("Server: Setsockopt");
            return false;
        }

        if(bind( mSocket.fileDescriptor, current->ai_addr, current->ai_addrlen) == -1)
        {
            close(mSocket.fileDescriptor);
            perror("Server: Bind");
            current = serverInfo->ai_next;
            continue;
        }
        
        // If we get here we successfuly binded a socket, so exit
        break;
    }

    if(current == nullptr)
    {
        fprintf(stderr, "Server: failed to bind\n" );
        return false;
    }

    // This is ugly but unfortunately necessary in order to get the ip address to a string
    void* addr = &( ( (struct sockaddr_in* ) current->ai_addr )->sin_addr );
    inet_ntop(current->ai_family, addr, ipstr, sizeof(ipstr) );
    mServerAddrInfo.ipAddr = ipstr;

    // Free structure
    freeaddrinfo(serverInfo);

    if( listen(mSocket.fileDescriptor, 10) == -1)
    {
        perror("Server: Listen");
        return false;
    }

    std::cout << "Server: Initialized TCP Socket... Ready to listen for Connections" << std::endl;

    mStatus = SocketUtils::Status::INITIALIZED;

    return true;
}

// TODO: FILL THIS OUT
bool NetworkServer::initializeUDPSocket()
{


    return true;
}

void NetworkServer::start()
{
    if(mStatus != SocketUtils::Status::INITIALIZED)
    {
        std::cout << "The server was NOT in the INITIALIZED state to start... " << std::endl;
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
            perror("Server: accept");
            std::cout << "Failed to accept client request " << std::endl;
        }

        std::cout << "Received a connection! " << std::endl;
        logClientInformation(clientSocketFD, clientAddrSize);

        handleRequest(clientSocketFD);
        std::cout << "Client Dealt with" << std::endl;
    }

    mStatus = SocketUtils::Status::INITIALIZED;
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

    mStatus = SocketUtils::Status::INITIALIZED;
    return;
}

void NetworkServer::handleRequest(int clientSocketFD)
{
    char rBuffer[100] = "";
    char sBuffer[100] = "";
    int bytes = 0;

    while(1)
    {
        bytes = recv(clientSocketFD, rBuffer, sizeof(rBuffer), 0);
        if(bytes == -1)
        {
            perror("Server: Recv");
            break;
        }

        if(bytes == 0)
        {
            std::cout << "Client closed the connection " << std::endl;
            break;
        }

        rBuffer[bytes] = '\0';
        printf("Server: Received '%s' \n", rBuffer);
    }

    close(clientSocketFD);
    return;
}

// UTILITY FUNCTIONS BELOW ------------------------------------

void NetworkServer::logClientInformation(int clientSocketFD, socklen_t clientAddrSize)
{
    struct sockaddr_in address;
    int ret = getpeername(clientSocketFD, (struct sockaddr*)&address, &clientAddrSize);
    if(ret == -1)
    {
        perror("Server: getpeername");
        return;
    }

    char hostName[INET6_ADDRSTRLEN];
    getnameinfo((struct sockaddr*)&address, sizeof(address), hostName, sizeof( hostName), NULL, 0, 0);

    std::string host = hostName;
    std::cout << "Client Name: " << host << std::endl;

    // inet_ntop(current->ai_family, addr, ipstr, sizeof(ipstr) );
    // inet_ntop(address.sin_family, )

    return;
}

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
