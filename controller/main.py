import socket
import sys
from types import FunctionType
from enum import IntEnum

VERSION = '0.2'

class ControllerMode(IntEnum):
    CONNECTION_MODE = 0  # Меню выбора консоли
    CONSOLE_MODE = 1     # Работа с конкретной консолью
    STAT_MODE = 2        # Статистика (заглушка)
    EXIT_MODE = 3        # Выход

class MainController:
    __instance = None
    __initialized = False
    
    def __new__(cls):
        if cls.__instance is None:
            cls.__instance = super().__new__(cls)
        return cls.__instance

    def __init__(self):
        if self.__initialized:
            return
        
        self.__current_mode = ControllerMode.CONNECTION_MODE
        self.__handlers: dict[int, FunctionType] = {
            ControllerMode.CONNECTION_MODE: self.handle_connection_mode,
            ControllerMode.CONSOLE_MODE: self.handle_console_mode,
            ControllerMode.STAT_MODE: self.handle_stat_mode,
        }
        self.__sock = None
        self.__server_host = '127.0.0.1'
        self.__server_port = 9091
        self.__initialized = True

    def run(self):
        print(f"RemoteConsoleController v{VERSION}")
        print("="*30)
        
        try:
            while self.__current_mode != ControllerMode.EXIT_MODE:
                try:
                    self.__handlers[self.__current_mode]()
                except KeyboardInterrupt:
                    print("\nПрервано пользователем")
                    break
                except Exception as e:
                    print(f"Ошибка: {e}")
                    self.__sock = None  # Сброс соединения при ошибке
        finally:
            self.cleanup()

    def connect(self, port=None):
        """Устанавливает соединение с сервером"""
        if self.__sock:
            self.__sock.close()
        
        target_port = port if port else self.__server_port
        try:
            self.__sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.__sock.settimeout(5.0)  # Таймаут на подключение
            self.__sock.connect((self.__server_host, target_port))
            print(f"Подключено к {self.__server_host}:{target_port}")
            return True
        except Exception as e:
            print(f"Не удалось подключиться: {e}")
            self.__sock = None
            return False

    def handle_connection_mode(self):
        """Меню выбора консоли"""
        print("\n--- ГЛАВНОЕ МЕНЮ ---")
        print("1. Подключиться к консоли (Port 9091)")
        print("2. Подключиться к консоли (Port 9092)")
        print("3. Статистика")
        print("4. Выход")
        
        choice = input("Выберите действие: ")
        
        if choice == '1':
            if self.connect(9091):
                self.__current_mode = ControllerMode.CONSOLE_MODE
        elif choice == '2':
            if self.connect(9092):
                self.__current_mode = ControllerMode.CONSOLE_MODE
        elif choice == '3':
            self.__current_mode = ControllerMode.STAT_MODE
        elif choice == '4':
            self.__current_mode = ControllerMode.EXIT_MODE

    def handle_console_mode(self):
        """Отправка команд на сервер"""
        if not self.__sock:
            print("Нет соединения. Возврат в меню...")
            self.__current_mode = ControllerMode.CONNECTION_MODE
            return

        try:
            cmd = input("cmd> ")
            if cmd.lower() in ['exit', 'quit', 'back']:
                self.__current_mode = ControllerMode.CONNECTION_MODE
                return
            
            if cmd:
                self.__sock.send(cmd.encode('utf-8'))
                # Получаем ответ
                data = self.__sock.recv(4096)
                if data:
                    print(data.decode('utf-8'))
                else:
                    print("Сервер разорвал соединение")
                    self.__sock = None
        except BrokenPipeError:
            print("Соединение разорвано сервером")
            self.__sock = None
        except Exception as e:
            print(f"Ошибка связи: {e}")
            self.__sock = None

    def handle_stat_mode(self):
        """Заглушка для статистики"""
        print("\n--- СТАТИСТИКА ---")
        print("Пока не реализовано на сервере")
        input("Нажмите Enter для возврата...")
        self.__current_mode = ControllerMode.CONNECTION_MODE

    def cleanup(self):
        if self.__sock:
            self.__sock.close()
        print("Контроллер завершен")

if __name__ == "__main__":
    controller = MainController()
    controller.run()