#ifndef __MAIN_HEADER_H__
#define __MAIN_HEADER_H__

/**
 *
 * This header file contains all the libraries,namespaces,
 * type definitions and parameters used
 *
 **/

/* Libraries */
// NETWORK - SOCKETS
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

// UNIX
#include <unistd.h>
#include <errno.h>

// C
#include <string.h>

// STL
#include <iostream>
#include <iomanip>

#include <string>
#include <vector>
#include <map>

#include <filesystem>
#include <fstream>

#include <thread>

#include <chrono>

#include <iostream>
#include <thread>
#include <ctime>

// OPENSSL
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>

/* Usings */
using std::cin;
using std::cout;
using std::endl;

using std::ifstream;
using std::ofstream;

using std::map;
using std::string;
using std::vector;

using std::thread;

using std::setfill;
using std::setw;

// Namespaces
namespace fs = std::filesystem;

/* Typedefs */
// Connection
typedef struct sockaddr_in sockaddr_in;

// Program
typedef unsigned int uint;
typedef long unsigned int luint;

/**** Configurations ****/
// Sizes
#define MAX_MESSAGE_SIZE 1024

// Connection
#define PORT 5000

// Commands

// Directories
#define RESOURCES_DIR "resources"
#define REQUESTS_DIR "requests"

// States

#endif // !__MAIN_HEADER_H__