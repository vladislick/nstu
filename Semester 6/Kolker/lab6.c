#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <stddef.h>
#include <zlib.h>
#include <string.h>
#include <assert.h>
#include <zmq.h>


int main (void)
{
    char message[8102];
    int rc, ret;
    
    void *context;
    context = zmq_init (2); if(context==NULL) exit(0); // создали контекст
    
    void *subscriber = zmq_socket (context, ZMQ_SUB); assert (context); // создали 0mq гнездо- subscriber 
    
    ret = zmq_connect(subscriber, "tcp://192.168.2.179:5569");
    if (ret < 0) {
        perror("Error ZMQ socket connect");
        return 1;
    }

    ret = zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, "EN", 2);
    assert (ret == 0);

    printf("Waiting for the messages...\n");
    while (1)
    {
        memset(message, 0, 8102);
        rc = zmq_recv (subscriber, message, 8102, 0);
    
        printf("Received message:\n");
        printf("%s\n", message);
    }
    
    zmq_ctx_destroy(context);
    zmq_close (subscriber);
    return 0;
}