#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>

#pragma comment(lib, "ws2_32.lib")

const int DEFAULT_PORT = 27015;
const int BUFFER_SIZE = 1024;

std::vector<SOCKET> clients;
std::mutex clientsMutex;

void HandleClient(SOCKET clientSocket);

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock" << std::endl;
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket" << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddress; //для ipv4 предоставления протокола
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(DEFAULT_PORT);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR) {
        std::cerr << "Failed to bind socket" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Failed to listen on socket" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
    }

    std::cout << "Server is listening on port " << DEFAULT_PORT << std::endl;

    while (true) {
        SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Failed to accept client connection" << std::endl;
            closesocket(serverSocket);
            WSACleanup();
            return 1;
        }

        std::lock_guard<std::mutex> lock(clientsMutex);
        clients.push_back(clientSocket);

        std::thread clientThread(HandleClient, clientSocket);
        clientThread.detach();
    }

    closesocket(serverSocket);
    WSACleanup();

    return 0;
}

void HandleClient(SOCKET clientSocket) {
    char buffer[BUFFER_SIZE];
    int bytesRead;

    do {
        bytesRead = recv(clientSocket, buffer, BUFFER_SIZE, 0);
        if (bytesRead > 0) {
            buffer[bytesRead - 1] = '\0';
            std::cout << buffer << std::endl;

            std::lock_guard<std::mutex> lock(clientsMutex);
            for (SOCKET otherClient : clients) {
                send(otherClient, buffer, strlen(buffer), 0);
            }
        }
    } while (bytesRead > 0);

    std::cout << "Client disconnected" << std::endl;
    std::lock_guard<std::mutex> lock(clientsMutex);
    clients.erase(std::remove(clients.begin(), clients.end(), clientSocket), clients.end());

    closesocket(clientSocket);
}