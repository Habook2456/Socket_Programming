#ifndef __TCP_CLIENT__
#define __TCP_CLIENT__

#include "main_header.h"

/**
 *
 * @brief Implementation of the TCP client class for the LiveSocket Chat
 *
 * @functionalities
 * TCP Client is initialized with an IP and a port
 *
 */
class TCP_Client
{
public:
    /* Constructors and Destructor*/
    TCP_Client(char *_ip, uint _port);
    ~TCP_Client();

    /* Attributes */

    /** Methods **/
    /* Senders */
    bool send_Block();

    /* Receivers */
    bool recv_Block();

    /* Utilities */
    void print_Information();

private:
    /* Attributes */
    int socket_FD;
    uint port;

    char *nickname;
    char *ip;

    sockaddr_in stsocketAddr;
};

#endif // !__TCP_CLIENT__