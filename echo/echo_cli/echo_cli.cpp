#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#define ECHO_PORT 7
#define BUFFER_SIZE 1024
int main(int argc, char* argv[])
{
    if(argc != 2) {
        std::cerr << "Использование: " << argv[0] << " <IP-адрес сервера>" << std::endl;
        return 1;
    }
    const char* server_ip = argv[1];
    // Создание TCP сокета
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) {
        perror("Ошибка создания сокета");
        return 1;
    }
    // Настройка адреса сервера
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(ECHO_PORT);
    if(inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        std::cerr << "Неверный IP-адрес: " << server_ip << std::endl;
        close(sockfd);
        return 1;
    }
    // Подключение к серверу
    if(connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Ошибка подключения к серверу");
        close(sockfd);
        return 1;
    }
    std::cout << "Подключено к " << server_ip << ":" << ECHO_PORT << std::endl;
    std::cout << "Введите текст для отправки (или 'exit' для выхода):" << std::endl;
    char send_buffer[BUFFER_SIZE];
    char recv_buffer[BUFFER_SIZE];
    while(true) {
        std::cout << "> ";
        std::cin.getline(send_buffer, BUFFER_SIZE);
        // Проверка на выход
        if(strcmp(send_buffer, "exit") == 0) {
            std::cout << "Завершение работы..." << std::endl;
            break;
        }
        // Отправка данных
        ssize_t sent = send(sockfd, send_buffer, strlen(send_buffer), 0);
        if(sent < 0) {
            perror("Ошибка отправки данных");
            break;
        }
        // Получение эхо-ответа
        memset(recv_buffer, 0, BUFFER_SIZE);
        ssize_t received = recv(sockfd, recv_buffer, BUFFER_SIZE - 1, 0);
        if(received < 0) {
            perror("Ошибка получения данных");
            break;
        }
        if(received == 0) {
            std::cout << "Сервер закрыл соединение" << std::endl;
            break;
        }
        recv_buffer[received] = '\0';
        std::cout << "Эхо: " << recv_buffer << std::endl;
    }
    close(sockfd);
    return 0;
}