#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <iostream>
#include <thread>
#include <ctime>
#include <chrono>
#include <vector>
#include <map>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>

#define STR_LENGTH 256

using namespace std;

map<char, string> actions = {
	{'M', "User message"},
	{'S', "Server message"},
};

string global_response;

string complete_digits(int t, int digits)
{
	std::ostringstream oss;
	oss << std::setw(digits) << std::setfill('0') << t;
	return oss.str();
}

// Función para calcular el hash SHA-256 de un archivo
std::string calculateSHA256(string filename)
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
	bio = BIO_new(BIO_s_mem());

	BIO_push(b64, bio);
	BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
	BIO_write(b64, input, length);
	BIO_flush(b64);
	BIO_get_mem_ptr(b64, &bufferPtr);

	char *encoded_data = (char *)malloc(bufferPtr->length);
	memcpy(encoded_data, bufferPtr->data, bufferPtr->length - 1);
	encoded_data[bufferPtr->length - 1] = '\0';

	BIO_free_all(b64);
	return encoded_data;
}

#include <openssl/bio.h>
#include <openssl/buffer.h>

unsigned char *base64_decode(const char *input, int length, int *output_length)
{
	BIO *bio, *b64;
	unsigned char *buffer = (unsigned char *)malloc(length);
	memset(buffer, 0, length);

	b64 = BIO_new(BIO_f_base64());
	bio = BIO_new_mem_buf(input, length);
	BIO_push(b64, bio);
	BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
	*output_length = BIO_read(b64, buffer, length);

	BIO_free_all(b64);
	return buffer;
}

void readMessage(int SocketFD)
{
	int n, size_friend_nick, size_message, size_clientsList;
	char option, bufferRead[STR_LENGTH];

	while (1)
	{

		n = recv(SocketFD, bufferRead, sizeof(bufferRead), 0);
		bufferRead[n] = '\0';
		option = bufferRead[0];

		time_t time_message = chrono::system_clock::to_time_t(chrono::system_clock::now());

		if (option == 'S')
		{
			string serverMessage = string(bufferRead).substr(4, n - 4);
			cout << "\n[Server message] " << serverMessage << " at " << ctime(&time_message) << '\n';
		}
		else if (option == 'M')
		{
			// parse size of friend's nickname and size of message
			string size_friend_nick_str(bufferRead, 1, 2);
			size_friend_nick = atoi(&size_friend_nick_str.front());
			string size_message_str(bufferRead, 3 + size_friend_nick, 3);
			size_message = atoi(&size_message_str.front());

			// parse friend's nickname and message
			string nickname_friend(bufferRead, 3, size_friend_nick);
			string message(bufferRead, 3 + size_friend_nick + 3, size_message);

			cout << "\n\n[Message from " << nickname_friend << "] at " << ctime(&time_message) << '\n'
				 << "->" << message << '\n';
		}
		else if (option == 'L')
		{
			// parse size of client list
			string size_clientsList_str(bufferRead, 1, 2);
			size_clientsList = atoi(&size_clientsList_str.front());

			// parse client's nick list
			string clientList_str(bufferRead, 3, n - 3);

			vector<string> clientList;

			stringstream ss(clientList_str);
			string token;

			while (getline(ss, token, ','))
			{
				clientList.push_back(token);
			}

			cout << "Lista de nombres de clientes:" << endl;

			for (const string &name : clientList)
			{
				cout << "- " << name << endl;
			}
		}
	}
	shutdown(SocketFD, SHUT_RDWR);
	close(SocketFD);
}

int main(int argc, char *argv[])
{
	struct sockaddr_in stSockAddr;
	int Res;
	int SocketFD = socket(AF_INET, SOCK_STREAM, 0); // IPPROTO_TCP
	int n;
	char option;

	if (-1 == SocketFD)
	{
		perror("cannot create socket");
		exit(EXIT_FAILURE);
	}

	memset(&stSockAddr, 0, sizeof(struct sockaddr_in));

	stSockAddr.sin_family = AF_INET;
	stSockAddr.sin_port = htons(45000);
	Res = inet_pton(AF_INET, "127.0.0.1", &stSockAddr.sin_addr);

	if (0 > Res)
	{
		perror("error: first parameter is not a valid address family");
		close(SocketFD);
		exit(EXIT_FAILURE);
	}
	else if (0 == Res)
	{
		perror("char string (second parameter does not contain valid ipaddress");
		close(SocketFD);
		exit(EXIT_FAILURE);
	}

	if (-1 == connect(SocketFD, (const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))
	{
		perror("connect failed");
		close(SocketFD);
		exit(EXIT_FAILURE);
	}

	string nickname, block, nick_friend, message, file_name;

	thread(readMessage, SocketFD).detach();

	// Nickname Option -> "N"
	option = 'N';
	cout << "Write your nickname to get started: ";
	cin >> nickname;
	block = option + complete_digits(nickname.size(), 2) + nickname;

	n = send(SocketFD, &(block.front()), block.size(), 0);
	block.clear();

	sleep(2);

	cout << "Chat Options:" << endl;
	cout << "1. Send a message (M)" << endl;
	cout << "2. Broadcast a message (W)" << endl;
	cout << "3. Send a file (F)" << endl;
	cout << "4. List users (L)" << endl;
	cout << "5. Quit (Q)" << endl;

	while (option != 'R')
	{
		cout << "Your option: ";
		cin >> option;

		if (option == 'M')
		{
			cout << "Enter your friend's nickname: ";
			cin >> nick_friend;

			cout << "Type your message: ";
			cin.ignore();
			getline(cin, message);

			block = option + complete_digits(nick_friend.size(), 2) + nick_friend + complete_digits(message.size(), 3) + message;
			n = send(SocketFD, block.c_str(), block.size(), 0);

			block.clear();
			message.clear();
			nick_friend.clear();
		}
		else if (option == 'C')
		{
			system("clear");
		}
		else if (option == 'W')
		{
			cout << "Write your message: ";
			cin.ignore();
			getline(cin, message);
			block = "W" + complete_digits(message.size(), 2) + message;

			n = send(SocketFD, &(block.front()), block.size(), 0);

			block.clear();
		}
		else if (option == 'L')
		{
			block = "L" + complete_digits(0, 2);
			n = send(SocketFD, &(block.front()), block.size(), 0);

			block.clear();
		}
		else if (option == 'Q')
		{
			block = "Q" + complete_digits(0, 2);
			n = send(SocketFD, &(block.front()), block.size(), 0);

			block.clear();
			break;
		}
		else if (option == 'F')
		{
			cout << "Enter your friend's nickname: ";
			cin >> nick_friend;

			cout << "Enter the file name: ";
			cin.ignore();
			getline(cin, file_name);

			ifstream file(file_name, ios::in | ios::binary);

			if (!file.is_open())
			{
				cout << "File not found" << endl;
				continue;
			}

			file.seekg(0, std::ios::end);
			std::streampos file_size = file.tellg();
			cout << "TAMANO DEL ARCHIVO: " << file_size << endl;
			file.seekg(0, std::ios::beg);

			cout << "Start sending file" << endl;
			int count = 0;
			char file_buffer[2048];
			file.read(file_buffer, sizeof(file_buffer));

			// Codificar el contenido del archivo en formato Base64
			char *encoded_data = base64_encode(reinterpret_cast<const unsigned char *>(file_buffer), file.gcount());

			block = option +
					complete_digits(nick_friend.size(), 2) +
					nick_friend +
					complete_digits(file_name.size(), 5) +
					file_name +
					complete_digits(strlen(encoded_data), 10) + // Usar strlen para obtener la longitud de la cadena codificada
					encoded_data;								// Usar la cadena codificada en lugar del archivo sin procesar

			// cout <<  endl << block << endl;
			// cout << "SHA: " << calculateSHA256(file_name) << endl;

			if (send(SocketFD, &(block.front()), block.size(), 0) == -1)
			{
				perror("Error al enviar el archivo");
				break;
			}

			cout << "Finish sending file" << endl;

			file.close();
			nick_friend.clear();
			block.clear();
			free(encoded_data);
		}
		else
		{
			cout << "Invalid option" << endl;
		}
		block.clear();
		message.clear();
	}

	printf("Closing chat ...\n *** Finished program ***\n ");

	shutdown(SocketFD, SHUT_RDWR);
	close(SocketFD);

	return 0;
}
