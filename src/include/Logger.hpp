#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <mutex>

using namespace std;

class Logger {
    fstream file; // Лучше хранить объект, а не указатель
    string name;
    inline static vector<Logger*> logger_list;
    inline static mutex list_mutex; // Для потокобезопасности

    static Logger* find_logger(const string& name) {
        for (auto& logger : logger_list) {
            if (logger->name == name) return logger;
        }
        return nullptr;
    }

public:
    static Logger* getLogger(const string& log_name) {
        lock_guard<mutex> lock(list_mutex);
        auto res = find_logger(log_name);
        if (res != nullptr) return res;

        Logger* new_logger = new Logger();
        new_logger->name = log_name;
        new_logger->file.open(log_name, ios::app);
        logger_list.push_back(new_logger);
        return new_logger;
    }

    void info(const string& msg) {
        string log_line = "[INFO]  " + msg + "\n";
        cout << log_line;
        if (file.is_open()) file << log_line;
    }

    void error(const string& msg) {
        string log_line = "[ERROR] " + msg + "\n";
        cerr << log_line;
        if (file.is_open()) file << log_line;
    }

    ~Logger() {
        if (file.is_open()) file.close();
    }
};