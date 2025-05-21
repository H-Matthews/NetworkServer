#include "Client.h"

#include <strings.h>
#include <string.h>
#include <unistd.h>

#include <iostream>

#define MAX 200

Client::Client(const std::string ipAddress, const std::string port) :
    mIpAddress(ipAddress),
    mPortNumber(port)
{ }

void Client::setup()
{
    mFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if(mFileDescriptor == -1)
    {
        std::cout << "Socket creation failed... " << std::endl;
        exit(0);
    }

    // Assign IP, PORT
    mServerAddress.sin_family = AF_INET;
    mServerAddress.sin_addr.s_addr = inet_addr( mIpAddress.c_str() );
    mServerAddress.sin_port = htons( std::stoi( mPortNumber ) );

    // Connect the client socket to server socket
    bool isConnected = false;
    while(! isConnected )
    {
        if ( connect (mFileDescriptor, (struct sockaddr*)&mServerAddress, sizeof(mServerAddress) ) != 0 )
        {
            std::cout << "Connection to server failed... " << std::endl;
            std::cout << "Will attempt reconnection again in five seconds " << std::endl;
            sleep(5);
        }
        else
        {
            isConnected = true;
        }
    }

    std::cout << "Connected to Server " << std::endl;

    // The below code should be eradicated quickly

    char buffer[MAX];
    int n;

    while( 1 )
    {
        bzero( &buffer, sizeof(buffer));
        std::cout << "Enter text: ";
        
        n = 0;
        while( (buffer[n++] = getchar() != '\n') );

        write(mFileDescriptor, buffer, sizeof(buffer));
        bzero(buffer, sizeof(buffer) );
        read(mFileDescriptor, buffer, sizeof(buffer) );

        printf("From Server: %s ", buffer);

        if( ( strncmp( buffer, "exit", 4 ) ) == 0 )
        {
            std::cout << "Client Disconnecting... " << std::endl;
            break;
        }
    }

    return;
}