#include <stdio.h>
#include <unistd.h> 
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

// Отправить сообщение message на адрес address и порт port
int sendMessage(const char* message, int bytes, int* udp_socket, const char* address, int port) {
    // Объявляем структуру с адресом назначения
    struct sockaddr_in dst_addr;
    // Очищаем эту структуру
    memset(&dst_addr, 0, sizeof(dst_addr));
	dst_addr.sin_family = AF_INET;
	dst_addr.sin_port = htons(port);
	dst_addr.sin_addr.s_addr = inet_addr(address);
    // Пробуем отправить пакет
    if (sendto(*udp_socket, message, bytes, 0, (const struct sockaddr *) &dst_addr, sizeof(dst_addr)) < 0)
        return -2;
    else
        return bytes;
}

// Принимает сообщение в message с адреса address и порта port
int recvMessage(char* message, int bytes, int* udp_socket, const char* address, int port) {
    // Объявляем структуру с адресом назначения
    struct sockaddr_in src_addr;
    int structlen = sizeof(struct sockaddr_in); 
    // Очищаем эту структуру
    memset(&src_addr, 0, sizeof(src_addr));
	src_addr.sin_family = AF_INET;
	src_addr.sin_port = htons(port);
	src_addr.sin_addr.s_addr = inet_addr(address);
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
    int udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    // Если сокет не удалось создать, вернуть -1
    if (udp_socket < 0) return -1;
    // Отправляем сообщения
    printf("Result code: %d\n", sendMessage(package, 82, &udp_socket, "127.0.0.1", 12345));
    printf("Waiting for an answer from server... ");
    sleep(1);
    if (recvMessage(package, 82, &udp_socket, "127.0.0.1", 12345) > 0)
        printf("received: %s.\n", package);
    else
        printf("cannot receive an answer.\n");
    return 0;
}

/*
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h> 
#include <netinet/in.h>
#include <termios.h>
#include <fcntl.h>
#include <string.h>
#include <arpa/inet.h>

// Создать пакет для отправки
void createPackage(char* package) {
    // Очистка пакета
	for (int i = 0; i < 82; i++) package[i] = 0;
    // Заполнение полей
    package[0] = 129;
	package[1] = 17;
	package[2] = 1;
	package[3] = 4;
    sprintf(&package[4],"Vdovin");
    sprintf(&package[41],"");
    sprintf(&package[61],"");

    // Вычисление контрольной суммы
	for (int i = 0; i < 81; i++) package[81] ^= package[i];
}

int main() {
	int udp_socket;
    int structlen = sizeof(struct sockaddr_in); 

    // Создание сокета
	if ((udp_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		printf("Cannot create socket\n");
        return -1;
	}

    // Создание сокета для отправки
	struct sockaddr_in dest_addr;
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(12345);
	dest_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Создаем пакет для отправки
	unsigned char package[81];
    createPackage(package);

    printf("Trying to send a package... ");
	if (sendto(udp_socket, (const char *)package, 82, 0, (const struct sockaddr *) &dest_addr, sizeof(dest_addr)) == -1) {
        printf("Error.\n");
        return -2;
    } else printf("Package sent.\n");

	printf("Trying to receive a package from the server...");
	sleep(1);
    int package_size = recvfrom(udp_socket, (char *)package, 82, 0, (struct sockaddr *) &dest_addr, &structlen);
	if (package_size > 0 || package_size == 0) printf("Received.\n");
    else {
        printf("Error.\n");
    }

    printf("Server answer is: %s\n", package);

	close(udp_socket);

	return 0;
} */