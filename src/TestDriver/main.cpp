#include <iostream>

#include "Client.h"

int main()
{
    Client myClient(1);

    std::cout << "Port Number: " << myClient.getPortNumber() << std::endl;

    return 0;
}