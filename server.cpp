#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <iostream>
#include <vector>
#include <map>
#include <thread>

#define STR_LENGTH 256

using namespace std;

char type_message;

map<char, string> actions = {{'N', "Nickname"},
							 {'M', "Message"},
							 {'B', "Broadcast"},
							 {'L', "List of users"},
							 {'R', "Close session"},
							 {'F', "Send a file"},
							 {'T', "Temp action"},
							 {'A', "Actions"}};

struct IClients
{
	int s_id;
	string s_name;
	IClients(int id, string name) : s_id(id), s_name(name) {}
};

// clients vector
vector<IClients> m_clients;

string complete_digits(int t, bool type)
{
	if (type)
	{
		if (t < 10)
			return ('0' + std::to_string(t));
		return std::to_string(t);
	}
	else
	{
		if (t < 10)
			return ("00" + std::to_string(t));
		else if (t < 100)
			return ('0' + std::to_string(t));
		return std::to_string(t);
	}
}

void processProtocol(IClients client)
{
	int n, m, size_friend_nick, size_message;
	char action, client_buffer[STR_LENGTH];
	string block;
	while (1)
	{
		int sd_friend = -1;
		n = recv(client.s_id, client_buffer, sizeof(client_buffer), 0);
		client_buffer[n] = '\0';
		action = client_buffer[0];

		if (action == 'M')
		{
			// parse size of friend's nickname and size of message
			string size_friend_nick_str(client_buffer, 1, 2);
			size_friend_nick = atoi(&size_friend_nick_str.front());
			string size_message_str(client_buffer, 3 + size_friend_nick, 3);
			size_message = atoi(&size_message_str.front());

			// parse friend's nickname and message
			string nickname_friend(client_buffer, 3, size_friend_nick);
			string message(client_buffer, 3 + size_friend_nick + 3, size_message);

			// search friend's socket descriptor
			for (IClients i : m_clients)
			{
				if (i.s_name == nickname_friend)
				{
					sd_friend = i.s_id;
					break;
				}
			}

			// if friend's socket descriptor is not found
			if (sd_friend == -1)
			{
				string response = "friend's nickname not found";
				block = "S" + complete_digits(response.size(), 0) + response;
				n = send(client.s_id, &(block.front()), block.size(), 0);

				cout << response << endl;
				continue;
			}

			// send message to friend
			block = "M" + complete_digits(client.s_name.size(), 1) + client.s_name + complete_digits(size_message, 0) + message;
			n = send(sd_friend, &(block.front()), block.size(), 0);

			block.clear();
			cout << client.s_name << " -> " << nickname_friend << endl;
		}
		else if (action == 'W')
		{
			cout << "Broadcast open" << endl;

			// parse message
			string message = string(client_buffer).substr(4, n - 4);

			// send message to all clients
			for (IClients i : m_clients)
			{
				if (i.s_id != client.s_id)
				{
					block = "M" + complete_digits(client.s_name.size(), 1) + client.s_name + complete_digits(message.size(), 0) + message;
					n = send(i.s_id, &(block.front()), block.size(), 0);
				}
			}
			block.clear();
			cout << "Broadcast close" << endl;
		}
		else if (action == 'L')
		{
			string clients;
			for (IClients i : m_clients)
			{
				if (i.s_name != client.s_name)
				{
					clients += i.s_name + ",";
				}
			}
			if (!clients.empty())
			{
				clients.pop_back();
			}

			block = "L" + complete_digits(m_clients.size(), 1) + clients;
			n = send(client.s_id, &(block.front()), block.size(), 0);
			block.clear();
		}
		else if (action == 'Q')
		{
			shutdown(client.s_id, SHUT_RDWR);
			close(client.s_id);

			// delete client from vector
			for (int i = 0; i < m_clients.size(); i++)
			{
				if (m_clients[i].s_id == client.s_id)
				{
					m_clients.erase(m_clients.begin() + i);
					break;
				}
			}

			cout << "Client " << client.s_name << " disconnected" << endl;
			break;
		}
	}

	shutdown(client.s_id, SHUT_RDWR);
	close(client.s_id);
}

int main(void)
{
	struct sockaddr_in stSockAddr;
	int SocketFD, n;

	struct sockaddr_in cli_addr;
	socklen_t client;

	if ((SocketFD = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("Socket");
		exit(1);
	}

	if (setsockopt(SocketFD, SOL_SOCKET, SO_REUSEADDR, "1", sizeof(int)) == -1)
	{
		perror("Setsockopt");
		exit(1);
	}

	memset(&stSockAddr, 0, sizeof(struct sockaddr_in));

	stSockAddr.sin_family = AF_INET;
	stSockAddr.sin_port = htons(45000);
	stSockAddr.sin_addr.s_addr = INADDR_ANY;

	if (bind(SocketFD, (struct sockaddr *)&stSockAddr, sizeof(struct sockaddr)) == -1)
	{
		perror("Unable to bind");
		exit(1);
	}

	if (listen(SocketFD, 5) == -1)
	{
		perror("Listen");
		exit(1);
	}

	char server_buffer[256];
	char client_buffer[STR_LENGTH];
	int tam;
	string block;
	string client_nick;
	for (;;)
	{
		client = sizeof(struct sockaddr_in);

		int clientFD = accept(SocketFD, (struct sockaddr *)&cli_addr, &client);

		n = recv(clientFD, client_buffer, sizeof(client_buffer), 0);
		client_buffer[n] = '\0';

		// process N option (nickname)

		client_nick = &client_buffer[3];
		IClients client(clientFD, client_nick);
		m_clients.push_back(client);
		cout << "New Client Registered [ " << client_nick << " ] with ClientFD [ " << clientFD << " ]\n";
		cout << "     Sending response\n";
		thread(processProtocol, client).detach();
	}

	close(SocketFD);
	return 0;
}