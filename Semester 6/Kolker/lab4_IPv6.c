#include <stdio.h>
#include <unistd.h> 
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

// Отправить сообщение message на адрес address и порт port
int sendMessage6(const char* message, int bytes, int* udp_socket, const char* address, int port) {
    // Объявляем структуру с адресом назначения
    struct sockaddr_in6 dst_addr;
    // Очищаем эту структуру
    memset(&dst_addr, 0, sizeof(dst_addr));
    // Заполняем ее данными
    dst_addr.sin6_family = AF_INET6;
    dst_addr.sin6_port = htons(port);
    // Пробуем преобразовать адрес, если не получилось, возвратить ноль
    if (inet_pton(AF_INET6, address, &dst_addr.sin6_addr) <= 0)
        return 0;
    // Пробуем отправить пакет
    if (sendto(*udp_socket, message, bytes, 0, (struct sockaddr *)&dst_addr, sizeof(dst_addr)) < 0)
        return -2;
    else
        return bytes;
}

// Принимает сообщение в message с адреса address и порта port
int recvMessage6(char* message, int bytes, int* udp_socket, const char* address, int port) {
    // Объявляем структуру с адресом назначения
    struct sockaddr_in6 src_addr;
    int structlen = sizeof(struct sockaddr_in); 
    // Очищаем эту структуру
    memset(&src_addr, 0, sizeof(src_addr));
	src_addr.sin6_family = AF_INET6;
	src_addr.sin6_port = htons(port);
	// Пробуем преобразовать адрес, если не получилось, возвратить ноль
    if (inet_pton(AF_INET6, address, &src_addr.sin6_addr) <= 0)
        return 0;
    // Пробуем отправить пакет
    if (recvfrom(*udp_socket, message, 82, 0, (struct sockaddr *) &src_addr, &structlen) < 0)
        return -2;
    else
        return bytes;
}

// Создать пакет для отправки
void createPackage(char* package) {
    // Очистка пакета
	memset(package, 0, 82);
    // Заполнение полей
    package[0] = 129;
	package[1] = 17;
	package[2] = 1;
	package[3] = 3;
    sprintf(&package[4],"Vdovin");
    sprintf(&package[41],"");
    sprintf(&package[61],"");

    // Вычисление контрольной суммы
	for (int i = 0; i < 81; i++) package[81] ^= package[i];
}

int main() {
    // Формируем сообщение для отправки
    char package[82];
    createPackage(package);
    // Создаем сокет
    int udp_socket = socket(AF_INET6, SOCK_DGRAM, 0);
    // Если сокет не удалось создать, вернуть -1
    if (udp_socket < 0) return -1;
    // Отправляем сообщения
    printf("Result code: %d\n", sendMessage6(package, 82, &udp_socket, "::1", 12345));
    printf("Waiting for an answer from server... ");
    sleep(1);
    if (recvMessage6(package, 82, &udp_socket, "::1", 12345) > 0)
        printf("received: %s.\n", package);
    else
        printf("cannot receive an answer.\n");
    return 0;
}