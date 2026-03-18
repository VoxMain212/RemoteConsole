#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>
#include <cstdio>
#include <cstring>
#include "Logger.hpp"

class Console {
    inline static int count = 0;
    bool running = true;
    int server_sock;
    Logger* logger;
    int port;

public:
    const int id;
    const string name;

    Console(string name, int port) : name{name}, id{count++}, port{port} {
        logger = Logger::getLogger("Console_" + to_string(id) + ".log");
        
        server_sock = socket(AF_INET, SOCK_STREAM, 0);
        if (server_sock < 0) {
            logger->error("Ошибка создания сокета");
            return;
        }

        int opt = 1;
        setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        sockaddr_in bind_addr{};
        bind_addr.sin_addr.s_addr = INADDR_ANY;
        bind_addr.sin_family = AF_INET;
        bind_addr.sin_port = htons(port); // Исправлено: было p ort

        if (bind(server_sock, (sockaddr*)&bind_addr, sizeof(bind_addr)) < 0) {
            logger->error("Ошибка bind");
            return;
        }

        listen(server_sock, SOMAXCONN);
        logger->info("Сервер запущен на порту " + to_string(port));
    }

    // Запуск в отдельном потоке
    void start() {
        thread(&Console::accept_loop, this).detach();
    }

    void accept_loop() {
        while (running) {
            sockaddr_in client_addr{};
            socklen_t client_len = sizeof(client_addr);
            
            // ПРИНИМАЕМ соединение
            int client_sock = accept(server_sock, (sockaddr*)&client_addr, &client_len);
            if (client_sock < 0) continue;

            logger->info("Клиент подключился");
            handle_client(client_sock);
            
            close(client_sock);
            logger->info("Клиент отключился");
        }
    }

    void handle_client(int client_sock) {
        char buff[4096];
        while (running) {
            // ЧИТАЕМ с клиентского сокета
            int res = recv(client_sock, buff, sizeof(buff) - 1, 0);
            if (res <= 0) break;

            buff[res] = '\0';
            string command(buff);
            
            // Убираем \n если есть
            if (!command.empty() && command.back() == '\n') command.pop_back();

            string result = handle_command(command);
            send(client_sock, result.c_str(), result.length(), 0);
        }
    }

    static string handle_command(const string& command) {
        if (command.empty()) return "";
        
        FILE* pipe = popen(command.c_str(), "r");
        if (pipe == NULL) return "Error: Failed to execute\n";

        string result;
        char buffer[4096];
        while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
            result += buffer; // Исправлено: было buf f
        }
        pclose(pipe);
        return result;
    }

    ~Console() {
        running = false;
        if (server_sock > 0) close(server_sock);
    }
};

class ConsoleManager {
    vector<Console*> console_list;
public:
    void add_console(Console* c) {
        console_list.push_back(c);
        c->start(); // Автозапуск
    }

    ~ConsoleManager() {
        for (auto& c : console_list) delete c;
    }
};