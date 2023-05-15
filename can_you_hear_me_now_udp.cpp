#include <arpa/inet.h>
#include <chrono>
#include <cstring>
#include <iostream>
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

void ServerThreadFunc(int port) {
    int server_socket;
    if ((server_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::cerr << "Error creating socket\n";
        return;
    }
    struct sockaddr_in servaddr = {0};
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);
    if (bind(server_socket, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        std::cerr << "Error binding socket\n";
        return;
    }
    char buffer[BUFFER_SIZE];
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int bytes = recvfrom(server_socket, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&client_addr, &client_len);
    buffer[bytes] = '\0';
    sendto(server_socket, "Your mother was a hamster", 36, 0, (struct sockaddr*)&client_addr, client_len);
    bytes = recvfrom(server_socket, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&client_addr, &client_len);
    buffer[bytes] = '\0';
    std::cout << "Client: " << buffer << "\n";
    sendto(server_socket, "Go away or I shall taunt you again", 36, 0, (struct sockaddr*)&client_addr, client_len);
    close(server_socket);
}

void ClientThreadFunction(int port) {
    int client_socket;
    if ((client_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::cerr << "Error creating socket\n";
        return;
    }
    struct sockaddr_in servaddr = {0};
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    if (inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported\n";
        return;
    }
    char buffer[BUFFER_SIZE];
    std::string message{""};
    int bytes;
    std::strcpy(buffer, message.c_str());
    sendto(client_socket, buffer, sizeof(buffer), 0, (struct sockaddr*)&servaddr, sizeof(servaddr));
    bytes = recvfrom(client_socket, buffer, BUFFER_SIZE, 0, NULL, NULL);
    buffer[bytes] = '\0';
    std::cout << "Server: " << buffer << "\n";
    message = "And your father smelled of elderberries";
    std::strcpy(buffer, message.c_str());
    sendto(client_socket, buffer, sizeof(buffer), 0, (struct sockaddr*)&servaddr, sizeof(servaddr));
    bytes = recvfrom(client_socket, buffer, BUFFER_SIZE, 0, NULL, NULL);
    buffer[bytes] = '\0';
    std::cout << "Server: " << buffer << "\n";
    close(client_socket);
}

int main() {
    int port = GetRandomPort();
    std::cout << "Random port: " << port << std::endl;
    std::thread server_thread(ServerThreadFunc, port);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ClientThreadFunction(port);
    server_thread.join();
    return 0;
}
