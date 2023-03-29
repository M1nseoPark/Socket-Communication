#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>

#define BUFSIZE 100
void error_handling(char* buf);

int main(int argc, char* argv[]) {

    int serv_sock, clnt_sock;
    struct sockaddr_in servAddr, clntAddr;
    struct timeval timeout;
    fd_set reads, cpyReads;

    socklen_t addrSz;
    int fdMax, strLen, fdNum, i;
    char buf[BUFSIZE];
    if (argc != 2) {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(atoi(argv[1]));

    if (bind(serv_sock, (struct sockaddr*)&servAddr, sizeof(servAddr)) == -1 )
        error_handling("bind() error");
    if (listen(serv_sock, 5) == -1)
        error_handling("listen() error");

    FD_ZERO(&reads);
    FD_SET(serv_sock, &reads);
    fdMax = serv_sock;

    while(1) {
        cpyReads = reads;
        timeout.tv_sec = 5;
        timeout.tv_usec = 5000;

        if ((fdNum = select(fdMax+1, &cpyReads, 0, 0, &timeout)) == -1 )
            break;
        if (fdNum == 0)
            continue;

        for (i = 0; i < fdMax+1; i++) {
            if (FD_ISSET(i, &cpyReads)) {
                if (i == serv_sock) {   // connection request!
                    addrSz = sizeof(clntAddr);
                    clnt_sock = accept(serv_sock, (struct sockaddr*)&clntAddr, &addrSz);
                    FD_SET(clnt_sock, &reads);

                if (fdMax < clnt_sock)
                    fdMax = clnt_sock;
                printf("connected client : %d\n", clnt_sock);
            }
            else {   // read message!
                strLen = read(i, buf, BUFSIZE);
                if (strLen == 0) {
                    FD_CLR(i, &reads);
                    close(i);
                    printf("close client : %d\n", i);
                }
                else {
                    write(i, buf, strLen);   // edho
                }
            }
        }
    }
    }
    close(serv_sock);
    return 0;
}

void error_handling(char *buf) {
    fputs(buf, stderr);
    //fputs('\n', stderr);
    exit(1);
}