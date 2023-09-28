/*****************************************************
 * Filename: server
 * Program: Server logs
 * Description: Terminal output of server processes
 * Type: .cpp
 * Date: 2022 December 18
 ******************************************************/

/* Headers */
#include "tcp_server.hpp"

/* Main */
int main()
{
    TCP_Server *myServer{new TCP_Server(const_cast<char *>("127.0.0.1"), PORT)};

    myServer->print_Information();

    cout << "    TCPServer waiting for data transmission \n";

    while (1)
    {
        for (;;)
        {
            myServer->process_Block(1);
        }

        // sleep(5);
    }

    cout << " 🏁 FINISH UDP SERVER PROGRAM 🏁\n";
}