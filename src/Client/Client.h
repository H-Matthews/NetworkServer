#pragma once

class Client
{
    public:
        Client(int portNumber);

        inline int getPortNumber() const;

    private:
        int mPortNumber;
};

 int Client::getPortNumber() const { return mPortNumber; }