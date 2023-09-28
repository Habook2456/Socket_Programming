# LiveSocket Connect

**LiveSocket Connect** is a project that implements a socket-based chat system using C/C++.

## Description

This project focuses on creating a client-server chat system using sockets to enable real-time communication among multiple clients. The server acts as an intermediary to route messages between connected clients. Clients can send and receive messages in real-time.

## Compilation Instructions

### Prerequisites

Make sure you have the following prerequisites installed on your system:

-   **g++ compiler**
-   **pthread library**

### Compilation Steps

1. Open a terminal and navigate to the root directory of the project.

2. Run the following command to compile both the client and server:
    ```bash
    make all
    ```
    This will generate two executables, server and client, in the bin directory.

## Usage

After compiling the client and server, you can execute them to start the chat application. Follow the steps below:

### Running the Server

To run the server, open a terminal and navigate to the project's root directory, then execute:

```bash
./bin/server
```

### Running Multiple Clients

To run multiple clients, open separate terminal windows and navigate to the project's root directory. For each client, execute:

```bash
./bin/client
```

You can run as many client instances as needed to connect to the server.

## Contribution

If you'd like to contribute to this project, we welcome collaborations! Feel free to fork the repository, make changes, and submit a pull request.

## License

This project is licensed under the MIT License.
