/*****************************************************
 * Filename: client
 * Program: client interface
 * Description: Terminal output for customer requests
 * Type: .cpp
 * Date: 2022 December 18
 ******************************************************/

/* Headers */
#include "tcp_client.hpp"

/* Main */
int main()
{
    TCP_Client *myClient{new TCP_Client(const_cast<char *>("127.0.0.1"), PORT)};
    string command{"Y"};

    sleep(5);
    myClient->print_Information();

    while (command != "N")
    {
        myClient->send_Block();
        cout << "Would like to retry? (Y/N)" << endl;
        cin >> command;
    }

    cout << " 🏁 FINISH UDP CLIENT PROGRAM 🏁\n";

    return 0;
}