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

class db
{
    public:
        db() {}

        db(std::string name, std::vector<std::string> initial_values)
            :name_(name), header(initial_values)
        {}

        void add_value(std::vector<std::string> new_value)
        {
            values.push_back(new_value);
            size++;
        }
        
        void remove_value(int index)
        {
            values.erase(values.begin() + index);
        }
        
        std::vector<std::vector<std::string>> get_values()
        {
            return values;
        }
        
        std::string get_name()
        {
            return name_;
        }

        int get_size()
        {
            return size;
        }

        int header_size()
        {
            return header.size();
        }
        std::vector<std::string> get_header()
        {
            return header;
        }

    private:
        std::string name_;
        std::vector<std::string> header;
        std::vector<std::vector<std::string>> values;
        int size = 0;
};
std::vector<db> dbs;

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

void print_line(std::vector<std::string> header)
{
    for (unsigned int i = 0; i < header.size(); i++)
    {
        std::cout << "+";
        std::cout << "-";
        for (unsigned int x = 0; x < header[i].size(); x++)
            std::cout << "-";
        std::cout << "-";
    }
    std::cout << "+";
    std::cout << std::endl;
}

void print_words(std::vector<std::string> header)
{
    for (unsigned int i = 0; i < header.size(); i++)
    {
        std::cout << "|" << " " << header[i] << " ";
    }
    std::cout << "|";
    std::cout << std::endl;
}

void db_sv(SOCKET cliente)
{
    int bytes_read = 0;
    char* buffer = (char*)calloc(1024, sizeof(char));
    std::cout << "Yayy" << std::endl; //CREATE {name} {val1} {val2} {val∞}...
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
    if (tokens[0].compare("create") == 0)
    {
        if (tokens.size() < 3)
        {
            std::cout << "Peticion de creacion invalida" << std::endl;
            return;
        }
        for (unsigned int i = 0; i < dbs.size(); i++)
        {
            if (dbs[i].get_name().compare(tokens[1]) == 0)
            {
                std::cout << "Una base de datos con este nombre ya ha sido creada" << std::endl;
                return;
            }
        }
        std::vector<std::string> buffer;
        std::string write_file;
        for (unsigned int i = 2; i < tokens.size(); i++)
        {
            buffer.push_back(tokens[i]);
            write_file.append(tokens[i]);
            if (i < tokens.size() - 1)
                write_file.append(" ");
        }
        dbs.push_back(db(tokens[1], buffer));
        std::ofstream db(tokens[1].append(".ldb"));
        db << write_file.append("\n");
        std::cout << "La base de datos se ha creado correctamente" << std::endl;
        db.close();
    }

    if (tokens[0].compare("add") == 0)
    {
        for (unsigned int i = 0; i < dbs.size(); i++)
        {
            if (dbs[i].get_name().compare(tokens[1]) == 0)
            {
                if (tokens.size() - 2 == dbs[i].header_size())
                {
                    std::vector<std::string> db_push;
                    std::string db_write;
                    for (unsigned int i = 2; i < tokens.size(); i++)
                    {
                        db_push.push_back(tokens[i]);
                        db_write.append(tokens[i]);
                        if (i < tokens.size() - 1)
                            db_write.append(" ");
                    }
                    dbs[i].add_value(db_push);
                    std::ofstream db_writee(tokens[1].append(".ldb"), std::ios_base::app);
                    db_writee << db_write.append("\n");
                }
                else
                {
                    std::cout << "La cantidad de datos introducidos es incorrecta";
                }
                return;
            }
        }
        std::cout << "No se ha encontrado una base de datos con ese nombre (ls para ver las bases de datos activas)" << std::endl;
    }

    if (tokens[0].compare("show") == 0)
    {
        for (unsigned int i = 0; i < dbs.size(); i++)
        {
            if (dbs[i].get_name().compare(tokens[1]) == 0)
            {
                std::vector<std::string> header = dbs[i].get_header();
                int count = 0;
                print_line(header);
                print_words(header);
                print_line(header);
                return;
            }
        }
    }

    if (tokens[0].compare("ls") == 0)
    {
        for (unsigned int i = 0; i < dbs.size(); i++)
        {
            std::cout << dbs[i].get_name();
            if (i < dbs.size() - 1)
                std::cout << ", ";
        }
        std::cout << std::endl;
    }
    if (tokens[0].compare("size") == 0) //doesnt work
    {
        for (unsigned int i = 0; i < dbs.size(); i++)
        {
            if (dbs[i].get_name().compare(tokens[1]) == 0)
            {
                std::cout << dbs[i].get_size() << std::endl;
                return;
            }
        }
        std::cout << "No se ha encontrado una base de datos con ese nombre (ls para ver las bases de datos activas)" << std::endl;
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
        std::cout << "> ";
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
