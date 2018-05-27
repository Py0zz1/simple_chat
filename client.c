#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <signal.h>

#define BUF_SIZE 1024

int main(int argc, char **argv)
{
    if(argc != 3)
    {
        printf("Using %s [IP] [PORT]\n",argv[0]);
        return -1;
    }
    int C_sock,chk;
    fd_set read_fd;
    char buf[BUF_SIZE];
    struct sockaddr_in C_addr;

    if((C_sock = socket(AF_INET,SOCK_STREAM,0)) < 0)
    {
        printf("SOCKET ERROR!\n");
        exit(0);
    }
    C_addr.sin_family = AF_INET;
    C_addr.sin_addr.s_addr = inet_addr(argv[1]);
    C_addr.sin_port = htons(atoi(argv[2]));

    if(connect(C_sock,(struct sockaddr *)&C_addr,sizeof(C_addr)) < 0)
    {
        printf("CONNECT ERROR!\n");
        exit(0);
    }
    printf("SERVER JOIN..!\n");
    FD_ZERO(&read_fd);

    while(1)
    {
        FD_SET(0,&read_fd);
        FD_SET(C_sock,&read_fd);

        if(select((C_sock+1),&read_fd,(fd_set *)0,(fd_set *)0,(struct timeval *)0) < 0)
        {
            printf("SELECT ERROR!\n");
            exit(0);
        }

        if(FD_ISSET(C_sock,&read_fd))
        {
            if((chk = recv(C_sock,buf,BUF_SIZE,0)) > 0)
            {
                buf[chk] = '\0';
                printf("\t\tSERVER SAY: %s",buf);
            }
        }
        if(FD_ISSET(0,&read_fd))
        {
            if(fgets(buf,BUF_SIZE,stdin) != NULL)
            {
                chk = strlen(buf);
                if(send(C_sock,buf,chk,0) < 0)
                {
                    printf("SEND ERROR!\n");
                    exit(0);
                }
                if(strcmp(buf,"exit\n") == 0)
                {
                    printf("EXIT NOW..!\n");
                    close(C_sock);
                    exit(0);
                }
            }
        }
    }
}
