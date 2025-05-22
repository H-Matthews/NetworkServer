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

        std::cout << "Connected to Server " << std::endl;

        businessLogic();
    }

    return;
}

void Client::businessLogic()
{
    int bytes = 0;

    std::string userText;
    std::string serverResponse;

    while( 1 )
    {
        std::cout << "Enter Text: ";
        std::getline(std::cin, userText);

        bytes = send( mFileDescriptor, userText.c_str(), userText.length(), 0 );

        if( bytes == -1)
        {
            perror("Server: Send");
            userText.clear();
        }

        if(bytes != (int)userText.length())
        {
            std::cout << "Bytes sent: " << bytes << std::endl;
            std::cout << "Total number of Bytes " << userText.length() << std::endl;
        }

        std::cout << "Bytes Sent: " << bytes << std::endl;
        std::cout << "Total number of Bytes in Response: " << userText.length() << std::endl;

        userText.clear();
    }

    return;
}