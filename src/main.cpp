#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include "include/Logger.hpp"
#include "include/Console.hpp"

using namespace std;

int main() {
    Logger* main_logger = Logger::getLogger("MainLogger.log");
    main_logger->info("Запуск сервера удаленной консоли");

    ConsoleManager manager;

    // Создаем консоли на разных портах
    manager.add_console(new Console("Admin", 9091));
    manager.add_console(new Console("User", 9092));

    main_logger->info("Сервер работает. Нажмите Ctrl+C для остановки");

    // Бесконечный цикл работы сервера
    try {
        while (true) {
            this_thread::sleep_for(chrono::seconds(1));
        }
    } catch (const std::exception& e) {
        main_logger->error(string("Exception: ") + e.what());
    }

    delete main_logger;
    return 0;
}