#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <WinSock2.h>
#include <vector>
#include <thread>
#include <ctime>
#include <chrono>
#include <Ws2tcpip.h>
#include <format>
#include <fstream>
#include <filesystem>
#include <algorithm>

const std::string SERVER_IP = "0.0.0.0";
//const char* key = generate_key();
int PORT = 5050;

std::vector<std::vector<std::string>> dbs;
std::vector<std::string> names;


std::vector<std::string> tokenize(std::string result)
{
    std::vector<std::string> tokens;
    std::stringstream check1(result);

    std::string intermediate;
    while (getline(check1, intermediate, ' '))
    {
        tokens.push_back(intermediate);
    }
    return tokens;
}

void db_sv(SOCKET cliente)
{
    int bytes_read = 0;
    char* buffer = (char*)calloc(1024, sizeof(char));
    std::cout << "Yayy" << std::endl; //CREATE {name} WHERE {val1} {val2} {val∞}...
    while (1)
    {
        bytes_read = recv(cliente, buffer, 1024, 0);
        if (bytes_read == -1)
        {
            break;
        }
        std::string result = buffer;
        std::vector<std::string> tokens = tokenize(result);
        for (unsigned int i = 0; i < tokens.size(); i++)
        {
            std::cout << tokens[i] << std::endl;
        }
        memset(buffer, 0, 1024);
    }

}

void evaluate(std::vector<std::string> tokens)
{
    if (tokens[0].compare("CREATE") == 0)
    {
        if (tokens.size() < 3)
        {
            std::cout << "Peticion de creacion invalida" << std::endl;
            return;
        }
        if (std::find(names.begin(), names.end(), tokens[1]) != names.end())
        {
            std::cout << "Una base de datos con este nombre ya ha sido creada" << std::endl;
            return;
        }
        names.push_back(tokens[1]);
        std::vector<std::string> buffer;
        std::string write_file;
        for (unsigned int i = 2; i < tokens.size(); i++)
        {
            buffer.push_back(tokens[i]);
            write_file.append(tokens[i]);
            write_file.append(" ");
        }
        std::ofstream db(tokens[1]);
        db << write_file;
        std::cout << "La base de datos se ha creado correctamente" << std::endl;
        db.close();
    }
}

void db_listen()
{
    int client_size;
    SOCKET listen_sock, client_sock;
    struct sockaddr_in server;
    listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listen_sock == INVALID_SOCKET) {
        printf("socket failed: %d\n", WSAGetLastError());
        WSACleanup();
        return;
    }
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(SERVER_IP.c_str());
    server.sin_port = htons(5050);
    if (bind(listen_sock, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("bind failed: %d\n", WSAGetLastError());
        closesocket(listen_sock);
        WSACleanup();
        return;
    }
    //printf("La base de datos localhost:5050\n");
    while (true)
    {
        listen(listen_sock, SOMAXCONN);
        client_size = sizeof(struct sockaddr_in);
        client_sock = accept(listen_sock, (struct sockaddr*)&server, &client_size);
        std::thread server_db(db_sv, client_sock);
        server_db.detach();
    }

    WSACleanup();
}

int main()
{
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("WSAStartup failed: %d\n", WSAGetLastError());
        return 1;
    }
    std::thread file_th(db_listen);
    file_th.detach();
    char msg[1024];
    while (true)
    {
        std::cout << ">";
        std::cin.getline(msg, 1024);
        if (msg[0] != NULL)
        {
            std::string test = msg;
            std::vector<std::string> tokens = tokenize(test);
            evaluate(tokens);
        }
    }
    return 0;
}
