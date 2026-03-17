#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>



using namespace std;


class Logger
{
    fstream* file;
    static vector<Logger*> logger_list;
    
    static Logger* find_logger(string name)
    {
        for (auto& logger : logger_list)
        {
            if (logger->name == name)
            {
                return logger;
            }
        }
        return 0;
    }
public:
    string name;
    static Logger* getLogger(string name)
    {
        auto res = find_logger(name);
        if (res != 0) {
            return res; 
        }
        Logger* new_logger = new Logger();
        new_logger->name = name;
        fstream* file = new fstream;
        file->open(name.c_str(), ios::ios_base::app);
        new_logger->file = file;
        logger_list.push_back(new_logger);
        return new_logger;
    }

    static void clear_logger(Logger* logger)
    {
        remove(logger_list.begin(), logger_list.end(), logger);
        delete logger;
    }

    static void rinfo(string& msg)
    {
        cout << "[INFO] " << msg << endl;
    }

    static void rerror(string& msg)
    {
        cout << "[ERROR] " << msg << endl;
    }

    void info(const char* msg)
    {
        cout << "[INFO] " << msg << endl;
        this->file->write(msg, sizeof(msg));
    }

    void error(string& msg)
    {   
        cout << "[ERROR] " << msg << endl;
        this->file->write(msg.c_str(), msg.length());
    }

    ~Logger()
    {
        delete this->file;
    }
};