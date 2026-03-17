#pragma once
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>


#include "Logger.hpp"


static Logger* console_logger = Logger::getLogger("Console.log");
static Logger* console_manager_logger = Logger::getLogger("ConsoleManager.log");


class Console
{
    static int count;
    bool running;
    int sock;

    public:
    const int id;
    const string name;
    const int port;

    Console(string name, int port) : name{name}, id{count}, port{port}
    {
        console_logger->info("Создана консоль");
        count += 1;
        this->sock = socket(AF_INET, SOCK_STREAM, 0);
        sockaddr_in bind_addr;
        bind_addr.sin_addr.s_addr = INADDR_ANY;
        bind_addr.sin_family = AF_INET;
        bind_addr.sin_port = htons(port);
        bind(sock, (sockaddr*)&bind_addr, sizeof(bind_addr));
        listen(sock, SOMAXCONN);
        console_logger->info("Привязка сокета");
    }

    void recv_command()
    {
        while (this->running)
        {
            char buff[4096];
            int res = recv(sock, buff, sizeof(buff), 0);
            if (res <= 0)
            {
                continue;
            }

            string result = handle_command(buff);
            
        }
    }

    static string handle_command(string command)
    {
        FILE* pipe = popen(command.c_str(), "r");

        if (pipe == NULL)
        {
            return NULL;
        }

        string result;
        char buff[4096];
        while (fgets(buff, sizeof(buff), pipe) != NULL)
        {
            result += buff;
        }
        return result;
    }
};


class ConsoleManager
{
    public:
    ConsoleManager()
    {

    }
};