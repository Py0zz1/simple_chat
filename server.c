#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/file.h>

#define BUF_SIZE 1024
#define MAX_CLIENT 5
#define LISTEN_SIZE 5

int main(int argc, char **argv)
{

    if(argc != 2)
    {
        printf("Using %s [PORT]\n",argv[0]);
        return -1;
    }
    int S_sock,C_sock[MAX_CLIENT],C_len,C_fd;
    int C_join_num=0,max_fd,chk;
    struct sockaddr_in S_addr,C_addr;
    fd_set read_fd;
    char buf[BUF_SIZE];

    if((S_sock = socket(AF_INET,SOCK_STREAM,0)) < 0 )
    {
        printf("SOCKET ERROR!\n");
        exit(0);
    }

    S_addr.sin_family = AF_INET;
    S_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    S_addr.sin_port = htons(atoi(argv[1]));

    if(bind(S_sock,(struct sockaddr *)&S_addr,sizeof(S_addr)) < 0)
    {
        printf("BIND ERROR!\n");
        exit(0);
    }
    if(listen(S_sock,LISTEN_SIZE) < 0)
    {
        printf("LISTEN ERROR!\n");
        exit(0);
    }
    max_fd = S_sock+1;
    FD_ZERO(&read_fd);
    printf("CLIENT JOIN WAITING...\n");
    while(1)
    {
        memset(buf,0,BUF_SIZE);
        if((C_join_num-1) >= 0) max_fd = C_sock[C_join_num-1]+1;
        FD_SET(S_sock,&read_fd);

        for(int i=0; i<C_join_num; i++)
            FD_SET(C_sock[i],&read_fd);

        if(select(max_fd,&read_fd,(fd_set *)0,(fd_set *)0,(struct timeval *)0) < 0)
        {
            printf("SELECT ERROR!\n");
            exit(0);
        }

        if(C_join_num < 5 && FD_ISSET(S_sock,&read_fd))
        {
            C_len = sizeof(C_addr);
            if((C_fd = accept(S_sock,(struct sockaddr *)&C_addr,(struct socklen_t *)&C_len)) < 0)
            {
                printf("ACCEPT ERROR!\n");
                exit(0);
            }

            C_sock[C_join_num] = C_fd;
            C_join_num++;


            printf("NUM(%d) CLIENT JOIN\n",C_join_num);
        }

        for(int i=0; i<C_join_num; i++)
        {
            if(FD_ISSET(C_sock[i],&read_fd))
            {

                if((chk = recv(C_sock[i],buf,BUF_SIZE,0)) > 0)
                {
                    buf[chk]='\0';
                }
                if(strcmp(buf,"exit\n")==0)
                {
                    printf("CLIENT EXIT..!\nREMIND CLIENT(%d)\n",i);
                    C_join_num--;
                    continue;
                }

                for(int j=0; j<C_join_num; j++)
                    send(C_sock[j],buf,chk,0);
                printf("RECV_MSG : %s\n",buf);
            }
        }
    }
    FD_ZERO(&read_fd);
    close(S_sock);
}
