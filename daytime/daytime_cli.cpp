#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#define DAYTIME_PORT 13
#define BUFFER_SIZE 256
int main(int argc, char* argv[])
{
    if(argc != 2) {
        std::cerr << "Использование: " << argv[0] << " <IP-адрес сервера>" << std::endl;
        return 1;
    }
    const char* server_ip = argv[1];
    // Создание UDP сокета
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0) {
        perror("Ошибка создания сокета");
        return 1;
    }
    // Настройка адреса сервера
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(DAYTIME_PORT);
    if(inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        std::cerr << "Неверный IP-адрес: " << server_ip << std::endl;
        close(sockfd);
        return 1;
    }
    // Отправка запроса (для daytime достаточно отправить любой байт)
    char request = ' ';
    ssize_t sent = sendto(sockfd, &request, sizeof(request), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if(sent < 0) {
        perror("Ошибка отправки запроса");
        close(sockfd);
        return 1;
    }
    std::cout << "Запрос отправлен на " << server_ip << ":" << DAYTIME_PORT << std::endl;
    // Получение ответа
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    struct sockaddr_in from_addr;
    socklen_t from_len = sizeof(from_addr);
    ssize_t received = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0, (struct sockaddr*)&from_addr, &from_len);
    if(received < 0) {
        perror("Ошибка получения ответа");
        close(sockfd);
        return 1;
    }
    buffer[received] = '\0';
	
	char sender_ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &from_addr.sin_addr, sender_ip, sizeof(sender_ip));
	uint16_t sender_port = ntohs(from_addr.sin_port);
	
    std::cout << "Получено от сервера: " << buffer << std::endl;
	std::cout << "IP-адрес, с которого получен ответ: " << sender_ip << ':' << sender_port << std::endl;
    close(sockfd);
    return 0;
}