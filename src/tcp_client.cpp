
/*
** 2023 September 25
**
** The design of a client chat protocol using socket programming
**
********************************************************************************
** The basic methods of the Client class shown in the lines above.
** From here the code is shown, to implement the client's protocol when sending
**
*/

#include "../include/tcp_client.hpp"

/* Constructors and Destructor */
TCP_Client::TCP_Client(char *_ip, uint _port)
{
    port = _port;
    ip = _ip;
    socket_FD = socket(AF_INET, SOCK_STREAM, 0);

    if (-1 == socket_FD)
    {
        perror("Cannot create socket");
        exit(EXIT_FAILURE);
    }

    memset(&stsocketAddr, 0, sizeof(struct sockaddr_in));

    stsocketAddr.sin_family = AF_INET;
    stsocketAddr.sin_port = htons(port);

    int valid_network_addr = inet_pton(AF_INET, ip, &stsocketAddr.sin_addr);

    if (0 > valid_network_addr)
    {
        perror("error: first parameter is not a valid address family");
        close(socket_FD);
        exit(EXIT_FAILURE);
    }

    else if (0 == valid_network_addr)
    {
        perror("char string (second parameter does not contain valid ipaddress");
        close(socket_FD);
        exit(EXIT_FAILURE);
    }

    if (-1 == connect(socket_FD, (const struct sockaddr *)&stsocketAddr, sizeof(struct sockaddr_in)))
    {
        perror("Connect failed");
        close(socket_FD);
        exit(EXIT_FAILURE);
    }
}

TCP_Client::~TCP_Client()
{
}

/* Methods */
// Senders
bool TCP_Client::send_Block()
{
    int bytes_send{0};
    char msj[] = "I'm groot";

    bytes_send = send(socket_FD, msj, strlen(msj), 0);

    std::cout << bytes_send << endl;

    return 1;
}

// Receivers
bool TCP_Client::recv_Block()
{
    return 1;
}

// Utilities
void TCP_Client::print_Information()
{
    system("clear");

    cout << "         ░░░░░░░░ LiveSocket Connect - Client ░░░░░░░░\n"
         << "******************************************************\n"
         << "          HOST          : " << ip << "\n"
         << "           PORT          : " << port << "\n"
         << "******************************************************"
         << endl;
}