#include <arpa/inet.h>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

constexpr int MIN_PORT = 2001;
constexpr int MAX_PORT = 65535;
constexpr int BUFFER_SIZE = 1024;

// Utility function to generate a random port number between MIN_PORT and MAX_PORT
int GetRandomPort() {
    srand(std::chrono::system_clock::now().time_since_epoch().count());
    return rand() % (MAX_PORT - MIN_PORT + 1) + MIN_PORT;
}
void serverThreadFunc(int port) {
    // Create a socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Failed to create socket\n";
        return;
    }
    // Set the server address
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY; // Accept connections on any interface
    serverAddr.sin_port = htons(port); // Server port number
    // Bind the socket to the server address
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Failed to bind socket\n";
        close(serverSocket);
        return;
    }
    // Listen for incoming connections
    if (listen(serverSocket, SOMAXCONN) == -1) {
        std::cerr << "Failed to listen for connections\n";
        close(serverSocket);
        return;
    }
    std::cout << "Server listening for connections on port " << port << "...\n";
    // Accept a client connection
    sockaddr_in clientAddr{};
    socklen_t clientAddrSize = sizeof(clientAddr);
    int client_socket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrSize);
    if (client_socket == -1) {
        std::cerr << "Failed to accept client connection\n";
        close(serverSocket);
        return;
    }
    // Send first message to the client
    const char* init_message = "Your mother was a hamster";
    if (send(client_socket, init_message, strlen(init_message), 0) == -1) {
        std::cerr << "Failed to send initial message to client\n";
    }
    // receive message from client
    char buffer[BUFFER_SIZE];
    int bytes_read = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_read < 0) {
        std::cerr << "Error receiving message from server" << std::endl;
    }
    buffer[bytes_read] = '\0';
    std::cout << "Client: " << buffer << std::endl;
    // Send final message to the client
    const char* final_message = "Go away or I shall taunt you again";
    if (send(client_socket, final_message, strlen(final_message), 0) == -1) {
        std::cerr << "Failed to send final message to client\n";
    }
    close(client_socket);
    close(serverSocket);
}
int main() {
    int port = GetRandomPort();
    std::cout << "Random port: " << port << std::endl;
    std::thread server_thread(serverThreadFunc, port);
    // Wait for the server to start up
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    // Create a socket
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        std::cerr << "Failed to create socket\n";
        return 1;
    }
    // Set the server address
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // Connect to the local loopback address
    serverAddr.sin_port = htons(port); // Server port number
    // Connect to the server
    if (connect(client_socket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Failed to connect to server\n";
        close(client_socket);
        return 1;
    }
    // Receive the message from the server
    char buffer[BUFFER_SIZE];
    int bytes_read = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_read < 0) {
        std::cerr << "Error receiving message from server" << std::endl;
        return 1;
    }
    buffer[bytes_read] = '\0';
    std::cout << "Server: " << buffer << std::endl;
    // Send the response to the server
    std::string message = "And your father smelled of elderberries";
    std::strcpy(buffer, message.c_str());
    send(client_socket, buffer, message.length(), 0);
    // Receive the final message from the server
    bytes_read = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_read < 0) {
        std::cerr << "Error receiving message from server" << std::endl;
        return 1;
    }
    buffer[bytes_read] = '\0';
    std::cout << "Server: " << buffer << std::endl;
    // Close the client socket
    close(client_socket);
    // Join the server thread
    server_thread.join();
    return 0;
}
