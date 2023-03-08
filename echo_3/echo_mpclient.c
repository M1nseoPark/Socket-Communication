#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFSIZE 30
void errorHandling(char* msg);
void readRoutine(int sock, char* buf);
void wrtieRoutine(int sock, char* buf);

int main(int argc, char* argv[]) {

    int sock;
    pid_t pid;
    char buf[BUFSIZE];
    struct sockaddr_in servAddr;

    if (argc != 3) {
        printf("Usage:%s <ip><port>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(argv[1]);
    servAddr.sin_port = htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr*)&servAddr, sizeof(servAddr)) == -1 )
        errorHandling("connect() error");

    pid = fork();
    if (pid == 0)
        wrtieRoutine(sock, buf);
    else
        readRoutine(sock, buf);

    close(sock);
    return 0;
}

void errorHandling(char* msg) {

    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}

void readRoutine(int sock, char* buf) {

    while(1) {
        int strLen = read(sock, buf, BUFSIZE);
        if (strLen == 0)
            return ;

        buf[strLen] = 0;
        printf("message from server: %s", buf);
    }
}

void wrtieRoutine(int sock, char* buf) {

    while(1) {
        fgets(buf, BUFSIZE, stdin);
        if (!strcmp(buf, "q\n") || !strcmp(buf, "Q\n")) {
            shutdown(sock, SHUT_WR);
            return;
        }
        write(sock, buf, strlen(buf));
    }
}