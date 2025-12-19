#define _POSIX_C_SOURCE 200112L

#include<stdio.h>
#include<stdbool.h>
#include<fcntl.h>
#include<unistd.h>

#include<sys/epoll.h>

#define MAX_EPOLL_EVENTS 16
#define MAX_INPUT_PIPES 4

int main(int argc, char const *argv[])
{
    int epollfd = -1;
    int eventsReady = 0;
    int inf[MAX_INPUT_PIPES][2];

    char message[] = "Hello There!";
    char receiveBuffer[4096];

    struct epoll_event einf[MAX_INPUT_PIPES];
    struct epoll_event eventQueue[MAX_EPOLL_EVENTS];

    epollfd = epoll_create1(0);
    if(epollfd == -1){
        printf("Couldn't create epoll instance!\n");
    }

    printf("Watching for types %d events.\n", EPOLLIN);

    for(int i=0 ; i<MAX_INPUT_PIPES ; i++){
        pipe(inf[i]);
        printf("fd for %d = %d, %d\n", i, inf[i][0], inf[i][1]);

        einf[i].events = EPOLLIN;
        einf[i].data.fd = inf[i][0];

        if(epoll_ctl(epollfd, EPOLL_CTL_ADD, inf[i][0], &einf[i]) == -1){
            printf("Couldn't add to epoll!");
        }

        if(i%2 == 0){
            write(inf[i][1], message, sizeof(message));
        }
    }

    while(true){
        // epoll_ctl(epollfd, EPOLL_CTL_DEL, inf[2][0], &einf[2]);
        eventsReady = epoll_wait(epollfd, eventQueue, MAX_EPOLL_EVENTS, -1);

        for(int i=0 ; i<eventsReady ; i++){
            printf("Got an event %d for %d!\n",
                eventQueue[i].events, eventQueue[i].data.fd);
            read(eventQueue[i].data.fd, receiveBuffer, sizeof(receiveBuffer));
            printf("Received buffer : %s\n", receiveBuffer);
        }
    }

    return 0;
}
