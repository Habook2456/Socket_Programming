#include "../include/tcp_server.hpp"

/* Constructors */
TCP_Server::TCP_Server(char *_ip, uint _port)
{
    port = _port;
    ip = _ip;

    if ((socket_FD = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket");
        exit(1);
    }

    if (setsockopt(socket_FD, SOL_SOCKET, SO_REUSEADDR, "1", sizeof(int)) == -1)
    {
        perror("Setsockopt");
        exit(1);
    }

    memset(&SockAddr, 0, sizeof(struct sockaddr_in));

    SockAddr.sin_family = AF_INET;
    SockAddr.sin_port = htons(port);
    SockAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(socket_FD, (struct sockaddr *)&SockAddr, sizeof(struct sockaddr)) == -1)
    {
        perror("Unable to bind");
        exit(1);
    }

    if (listen(socket_FD, 5) == -1)
    {
        perror("Listen");
        exit(1);
    }
}

/* Methods */
// Senders
bool TCP_Server::send_Block()
{
    return 1;
}

// Receivers
bool TCP_Server::process_Block(uint client_socket_FD)
{
    char *client_buffer = new char[10];

    int bytes_received{0}, client_FD = accept(socket_FD, (struct sockaddr *)&client_addr, &client);

    cout << "New Client Registered [ " << string(client_buffer) << " ] with ClientFD [ " << client_FD << " ]\n";

    bytes_received = recv(client_FD, client_buffer, 10, 0);

    cout << "MSG " << client_buffer << " " << bytes_received << client_socket_FD << endl;

    return 1;
}

// Utilities
void TCP_Server::print_Information()
{
    system("clear");

    cout << "         ░░░░░░░░ LiveSocket Connect - Server ░░░░░░░░\n"
         << "******************************************************\n"
         << "        IP            : " << ip << "\n"
         << "        PORT          : " << port << "\n"
         << "******************************************************"
         << endl;
}