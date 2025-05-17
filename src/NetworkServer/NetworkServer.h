#pragma once

class NetworkServer
{
    public:
        NetworkServer(int port);
        
        inline int getPortNumber() const;

    private:
        int mPortNumber;
};

int NetworkServer::getPortNumber() const { return mPortNumber; }
