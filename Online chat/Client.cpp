#include <iostream>
#include <winsock2.h>
#include <thread>

#pragma comment(lib, "ws2_32.lib")

void sendMessages(SOCKET clientSocket) {
    std::string message;
    while (true) {
        std::getline(std::cin, message);
        if (send(clientSocket, message.c_str(), message.size(), 0) == SOCKET_ERROR) {
            std::cerr << "Send failed.\n";
            break;
        }
    }
}

int main() {
    WSADATA wsaData;
    SOCKET clientSocket;
    sockaddr_in serverAddr;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed.\n";
        return 1;
    }

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed.\n";
        WSACleanup();
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345); // Порт сервера
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // IP-адрес сервера (localhost)

    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Connection failed.\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Connected to server!\n";

    std::thread senderThread(sendMessages, clientSocket);
    senderThread.detach();

    char buffer[1024];
    while (true) {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            std::cerr << "Connection closed or error occurred.\n";
            break;
        }
        buffer[bytesReceived] = '\0'; 
        std::cout << "Server: " << buffer << "\n";
    }

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}