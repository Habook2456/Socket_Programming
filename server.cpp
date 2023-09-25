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
#include <sstream>
#include <thread>
#include <chrono>
#include <iomanip>
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <fstream>

#define STR_LENGTH 4096

using namespace std;

char type_message;

struct IClients
{
	int s_id;
	string s_name;
	IClients(int id, string name) : s_id(id), s_name(name) {}
};

// clients vector
vector<IClients> m_clients;

string complete_digits(int t, int digits)
{
	std::ostringstream oss;
	oss << std::setw(digits) << std::setfill('0') << t;
	return oss.str();
}

// Función para calcular el hash SHA-256 de un archivo
std::string calculateSHA256(const char *filename)
{
	std::ifstream file(filename, std::ios::binary);
	if (!file)
	{
		perror("Error al abrir el archivo");
		exit(EXIT_FAILURE);
	}

	SHA256_CTX sha256;
	SHA256_Init(&sha256);

	char buffer[1024];
	while (file.read(buffer, sizeof(buffer)))
	{
		SHA256_Update(&sha256, buffer, file.gcount());
	}

	unsigned char hash[SHA256_DIGEST_LENGTH];
	SHA256_Final(hash, &sha256);

	std::string result;
	for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
	{
		char buf[3];
		snprintf(buf, sizeof(buf), "%02x", hash[i]);
		result += buf;
	}

	file.close();
	return result;
}

char *base64_encode(const unsigned char *input, int length)
{
	BIO *bio, *b64;
	BUF_MEM *bufferPtr;

	b64 = BIO_new(BIO_f_base64());
	BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
	bio = BIO_new(BIO_s_mem());
	bio = BIO_push(b64, bio);

	BIO_write(bio, input, length);
	BIO_flush(bio);
	BIO_get_mem_ptr(bio, &bufferPtr);

	char *encoded_data = (char *)malloc(bufferPtr->length);
	memcpy(encoded_data, bufferPtr->data, bufferPtr->length - 1);
	encoded_data[bufferPtr->length - 1] = '\0';

	BIO_free_all(bio);
	return encoded_data;
}

unsigned char *base64_decode(const char *input, int length, int *output_length)
{
	BIO *bio, *b64;
	unsigned char *buffer = (unsigned char *)malloc(length);
	memset(buffer, 0, length);

	b64 = BIO_new(BIO_f_base64());
	BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
	bio = BIO_new_mem_buf(input, length);
	bio = BIO_push(b64, bio);

	*output_length = BIO_read(bio, buffer, length);

	BIO_free_all(bio);
	return buffer;
}

void processProtocol(IClients client)
{
	int n, m, size_friend_nick, size_message, size_filename, size_content;
	char action, client_buffer[STR_LENGTH], file_buffer[1024];
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
				block = "S" + complete_digits(response.size(), 2) + response;
				n = send(client.s_id, &(block.front()), block.size(), 0);

				cout << response << endl;
				continue;
			}

			// send message to friend
			block = "M" + complete_digits(client.s_name.size(), 2) + client.s_name + complete_digits(size_message, 3) + message;
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
					block = "M" + complete_digits(client.s_name.size(), 2) + client.s_name + complete_digits(message.size(), 3) + message;
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

			block = "L" + complete_digits(m_clients.size(), 3) + clients;
			n = send(client.s_id, &(block.front()), block.size(), 0);
			block.clear();
		}
		else if (action == 'F')
		{
			cout << client_buffer << endl;
			cout << "file processing" << endl;
			// parse size of friend's nickname and size of message
			string size_friend_nick_str(client_buffer, 1, 2);
			size_friend_nick = atoi(&size_friend_nick_str.front());
			string size_filename_str(client_buffer, 3 + size_friend_nick, 5);
			size_filename = atoi(&size_filename_str.front());
			string size_content_str(client_buffer, 3 + size_friend_nick + 5 + size_filename, 10);
			size_content = atoi(&size_content_str.front());
			cout << "TAMANO DEL ARCHIVO: " << size_content << endl;
			// parse friend's nickname and message
			string nickname_friend(client_buffer, 3, size_friend_nick);
			string filename(client_buffer, 3 + size_friend_nick + 5, size_filename);
			string content(client_buffer, 3 + size_friend_nick + 5 + size_filename + 10, size_content);
			cout << content << endl;
			// Decodificar el contenido en Base64
			int decoded_length = 0;
			unsigned char *decoded_data = base64_decode(content.c_str(), content.length(), &decoded_length);

			// Agregar "_received" al nombre del archivo antes de la extensión
			size_t dotPos = filename.find_last_of(".");
			if (dotPos != string::npos)
			{
				filename = filename.substr(0, dotPos) + "_received" + filename.substr(dotPos);
			}
			else
			{
				filename = filename + "_received";
			}

			cout << "Modified filename: " << filename << endl;
			// Abrir un archivo para escribir los datos recibidos
			std::ofstream outputFile(filename, std::ios::binary);
			outputFile.write(reinterpret_cast<const char *>(decoded_data), decoded_length);

			/*
			n = recv(client.s_id, client_buffer, sizeof(client_buffer), 0);
			client_buffer[n] = '\0';
			action = client_buffer[0];
			*/

			cout << "file processed" << endl;
			outputFile.close();
			block.clear();
			free(decoded_data);
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