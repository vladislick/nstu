#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stddef.h>
#include <zlib.h>
#include <string.h>
#include <assert.h>
#include <zmq.h>

int main (void)
{
    char message[8102], messageUncompressed[8102];
    zmq_msg_t msg;
    int rc,ret;
    unsigned long int size = 8102;
    
    void *context = zmq_init(2); if(context==NULL) exit(0); // создали контекст
    
    void *subscriber = zmq_socket (context, ZMQ_SUB); assert (context); // создали 0mq гнездо- subscriber 
    
    ret = zmq_connect(subscriber, "tcp://192.168.2.179:5569");
    if (ret < 0) {
        perror("Error ZMQ socket connect");
        return 1;
    }

    ret = zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, "EN", 2);
    ret = zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, "x", 1);
    assert (ret == 0);

    printf("Waiting for the messages...\n");
    while (1)
    {
        memset(message, 0, 8102);
        rc = zmq_recv (subscriber, message, 8102, 0);
 
        if (message[0] == 'x') {
        	size = 8102;
        	memset(messageUncompressed, 0, 8102);
        	printf("Received compressed message with size %d:\n", rc);
        	uncompress(messageUncompressed, &size, message, sizeof(message));
        	printf("%s\n", messageUncompressed);
        } else {
        	printf("Received message with size %d:\n", rc);
        	printf("%s\n", message);
        }
    }
    
    return 0;
}
