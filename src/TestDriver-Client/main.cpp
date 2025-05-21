#include <iostream>

#include "Client.h"

int main()
{
    Client myClient( std::string("127.0.0.1"), std::string("8070") );
    myClient.setup();

    return 0;
}