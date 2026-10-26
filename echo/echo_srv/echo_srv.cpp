#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <signal.h>
#include <sys/socket.h>
#include <unistd.h>
#define ECHO_PORT 7777
#define BUFFER_SIZE 1024
#define BACKLOG 10
int server_sockfd = -1;
// Обработчик сигнала для корректного завершения
void signal_handler(int sig)
{
    std::cout << "\nЗавершение сервера..." << std::endl;
    if(server_sockfd >= 0) {
        close(server_sockfd);
    }
    exit(0);
}
int main(int argc, char* argv[])
{
    int port = ECHO_PORT;
    // Возможность указать порт через аргумент командной строки
    if(argc > 1) {
        port = atoi(argv[1]);
        if(port <= 0 || port > 65535) {
            std::cerr << "Неверный номер порта: " << argv[1] << std::endl;
            return 1;
        }
    }
    // Установка обработчика сигнала
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    // Создание TCP сокета
    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_sockfd < 0) {
        perror("Ошибка создания сокета");
        return 1;
    }
    // Установка опции SO_REUSEADDR для быстрого перезапуска
    int opt = 1;
    if(setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("Ошибка установки опции сокета");
        close(server_sockfd);
        return 1;
    }
    // Настройка адреса сервера
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    // Привязка сокета к адресу
    if(bind(server_sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Ошибка привязки сокета");
        close(server_sockfd);
        return 1;
    }
    // Начало прослушивания
    if(listen(server_sockfd, BACKLOG) < 0) {
        perror("Ошибка прослушивания");
        close(server_sockfd);
        return 1;
    }
    std::cout << "Echo TCP сервер запущен на порту " << port << std::endl;
    std::cout << "Ожидание подключений..." << std::endl;
    while(true) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        // Принятие подключения
        int client_sockfd = accept(server_sockfd, (struct sockaddr*)&client_addr, &client_len);
        if(client_sockfd < 0) {
            perror("Ошибка принятия подключения");
            continue;
        }
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
        std::cout << "Подключение от " << client_ip << ":" << ntohs(client_addr.sin_port) << std::endl;
        // Обработка клиента
        char buffer[BUFFER_SIZE];
        while(true) {
            memset(buffer, 0, BUFFER_SIZE);
            ssize_t received = recv(client_sockfd, buffer, BUFFER_SIZE - 1, 0);
            if(received < 0) {
                perror("Ошибка получения данных");
                break;
            }
            if(received == 0) {
                std::cout << "Клиент отключился" << std::endl;
                break;
            }
            buffer[received] = '\0';
            std::cout << "Получено: " << buffer;
            // Отправка эхо-ответа
            ssize_t sent = send(client_sockfd, buffer, received, 0);
            if(sent < 0) {
                perror("Ошибка отправки ответа");
                break;
            }
        }
        close(client_sockfd);
    }
    close(server_sockfd);
    return 0;
}