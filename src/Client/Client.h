#pragma once

#include <sys/socket.h>
#include <arpa/inet.h>

#include <string>

class Client
{
    public:
        Client(const std::string ipAddress, const std::string port);

        void setup();

    private:
        void businessLogic();

    private:
        const std::string mIpAddress;
        const std::string mPortNumber;

        int mFileDescriptor;
        struct sockaddr_in mServerAddress;
};