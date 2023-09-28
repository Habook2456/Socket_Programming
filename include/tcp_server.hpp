#ifndef __TCP_SERVER__
#define __TCP_SERVER__

#include "main_header.h"

/**
 *
 * @brief Implementation of the TCP server class for the LiveSocket Chat.
 *
 * @functionalities
 * TCP Server is initialized with an IP and a port
 *
 */
class TCP_Server
{
public:
    /* Constructors */
    TCP_Server(char *_ip, uint _port);
    ~TCP_Server();

    /** Methods **/
    /* Senders */
    bool send_Block();

    /* Receivers */
    bool process_Block(uint client_socket_FD);

    /* Utilities */
    void print_Information();

private:
    /* Variables */
    uint port;
    uint number_clients;

    int socket_FD;

    char *ip;

    sockaddr_in SockAddr;

    // Clients Connection
    sockaddr_in client_addr;
    socklen_t client = sizeof(sockaddr_in);
};

#endif // !__TCP_SERVER__