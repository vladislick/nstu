/*Программа сервера к лабораторной работе №4 */
/*2011 год*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h> 
#include<netinet/in.h>
#include <termios.h>
#include <fcntl.h>
#include <string.h>





 int udp_socket=-1; // идентификатор сокет для UDP
 char udpbuf[1024]; // буфер для хранения данных
 
struct sockaddr_in addr_UDP; //Структура для хранения адреса



void init_UDP(void);// инициализация UDP сокет
void send_UDP(int); // Отправляем данные в UDP


void main()
{

int i;
int recv;
int structlen;
char SENDER_ADDR[44];
int byte1,byte2,byte3,byte4;
unsigned char xorbyte=0;

struct sockaddr_in src_addr_UDP; //Структура для хранения отправителя

init_UDP(); // Инициализировали UDP сокет



while(1) // Вечный цикл
    {
    
    
    recv=recvfrom(udp_socket,udpbuf, 1024, 0,(struct sockaddr *)&src_addr_UDP,&structlen);
    printf("recv %d bytes: ",recv); // напечатали сколько байт пришло
    
    if(recv>0 && (unsigned char) udpbuf[0]==129) // если ключик верный смотрим кто послал
     {
    byte1=((int)src_addr_UDP.sin_addr.s_addr&0xff000000)>>24;
    byte2=((int)src_addr_UDP.sin_addr.s_addr&0xff0000)>>16;
    byte3=((int)src_addr_UDP.sin_addr.s_addr&0xff00)>>8;
    byte4=((int)src_addr_UDP.sin_addr.s_addr&0xff);   
    sprintf(SENDER_ADDR,"%d.%d.%d.%d",byte4,byte3,byte2,byte1);

    if( strlen(&udpbuf[4])<20 )
	{      
         printf("IP#%s BR#%d N1:%s\n",SENDER_ADDR,udpbuf[3],&udpbuf[4] ); // все верно
        }
    else
	{
	printf("IP#%s BR#%d N1:badname\n",SENDER_ADDR,udpbuf[3],&udpbuf[4] );// есть проблемы с посылкой
	}


    
	for(i=0;i<recv;i++) xorbyte=xorbyte^udpbuf[i]; //сделали исключаюшее или всех байтиков
	sprintf(udpbuf,"END_OF_WINTER!2022");// вот тут секретное словечко,которое будем отвечать в канал
	udpbuf[18]=(char)xorbyte;// контрольный XOR
	udpbuf[19]=0x0;//А вот тут будет секретный код,который мы будем посылать
	
	
	//отправили назад кодовое слово тому,кто нам прислал привет
    if(sendto(udp_socket,(char *)udpbuf,20,0,(const struct sockaddr *)&src_addr_UDP,sizeof(src_addr_UDP) ) ==-1)
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

    struct sockaddr_in addr; // структура с типом адресов


    udp_socket = socket(AF_INET, SOCK_DGRAM, 17); // инициализировали сокет
    if(udp_socket < 0)
    {
        perror("socket");
        exit(1);
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(12345);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    // привязали сокет
    if(bind(udp_socket, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        exit(2);
    }



}


