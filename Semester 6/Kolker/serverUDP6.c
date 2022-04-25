/*Программа сервера к лабораторной работе №4 */
/*2011 год*/
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

int udp_socket=-1; // идентификатор сокет для UDP
char udpbuf[1024]; // буфер для хранения данных
 
struct sockaddr_in6 addr_UDP6; //Структура для хранения адреса

void init_UDP(void);// инициализация UDP сокет
void send_UDP(int); // Отправляем данные в UDP


void *get_in_addr(struct sockaddr *sa){ 
    return &(((struct sockaddr_in6*)sa)->sin6_addr); 
}

void main()
{
int i;
int recv;
int structlen;
char SENDER_ADDR[INET6_ADDRSTRLEN];
int byte1,byte2,byte3,byte4;
unsigned char xorbyte=0;

struct sockaddr_in6 src_addr_UDP6; //Структура для хранения отправителя

init_UDP(); // Инициализировали UDP сокет

//udpbuf[0]=129;// ключик
//udpbuf[1]=29; // день
//udpbuf[2]=11; // месяц
//udpbuf[3]=1; // номер бригады
//sprintf(&udpbuf[4],"ivanov Ivan");
//sprintf(&udpbuf[41],"Petrov Petr");
//sprintf(&udpbuf[61],"Sidorov Sidr");



while(1) // Вечный цикл
    {
    
    // получили данные их сокет ждем такую посылку 
    //udpbuf[0]=129;// ключик
    //udpbuf[1]=29; // день
    //udpbuf[2]=11; // месяц
    //udpbuf[3]=1; // номер бригады
    //sprintf(&udpbuf[4],"ivanov Ivan");
    //sprintf(&udpbuf[41],"Petrov Petr");
    //sprintf(&udpbuf[61],"Sidorov Sidr");  
    
    memset(&udpbuf, 0, 1024); 
    memset(&src_addr_UDP6, 0, sizeof(struct sockaddr_in6)); 
    socklen_t structlen = sizeof(src_addr_UDP6);
    recv=recvfrom(udp_socket,udpbuf, 1024, 0,(struct sockaddr *)&src_addr_UDP6,&structlen);
    printf("recv %d bytes: ",recv); // напечатали сколько байт пришло
    
    if(recv>0 && (unsigned char) udpbuf[0]==129) // если ключик верный смотрим кто послал
     {
    inet_ntop(src_addr_UDP6.sin6_family,get_in_addr((struct sockaddr *)&src_addr_UDP6),SENDER_ADDR, sizeof SENDER_ADDR); 

    if( strlen(&udpbuf[4]) < 20 )
	{      
         printf("IP#%s BR#%d N1:%s %s\n",SENDER_ADDR,udpbuf[3],&udpbuf[4], &udpbuf[41] ); // все верно
        }
    else
	{
	printf("IP#%s BR#%d N1:badname\n",SENDER_ADDR,udpbuf[3] );// есть проблемы с посылкой
	}

    	for(i=0;i<recv;i++) xorbyte=xorbyte^udpbuf[i]; //сделали исключаюшее или всех байтиков
	sprintf(udpbuf,"password6");// вот тут секретное словечко,которое будем отвечать в канал
	udpbuf[18]=(char)xorbyte;// контрольный XOR
	udpbuf[19]=0x0;//А вот тут будет секретный код,который мы будем посылать
	
	//отправили назад кодовое слово тому,кто нам прислал привет
    if(sendto(udp_socket,(char *)udpbuf,20,0,(const struct sockaddr *)&src_addr_UDP6,sizeof(src_addr_UDP6) ) == -1)
        {
        perror("UDP_SEND");
        }
      }
// ждем 0.1 секунду
    usleep(100000); 
    }
}

void init_UDP(void) // инициализация сокет UDP
{
    struct sockaddr_in6 addr6; // структура с типом адресов

    udp_socket = socket(AF_INET6, SOCK_DGRAM, 17); // инициализировали сокет
    if(udp_socket < 0)
    {
        perror("socket");
        exit(1);
    }
    addr6.sin6_family = AF_INET6;
    addr6.sin6_port = htons(12345);
    addr6.sin6_addr = in6addr_any;
    
    // привязали сокет
    if(bind(udp_socket, (struct sockaddr *)&addr6, sizeof(addr6)) < 0)
    {
        perror("bind");
        exit(2);
    }
}

