#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFSIZE 30
void errorHandling(char* msg);
void readChildProc(int sig);

int main(int argc, char* argv[]) {

    int servSock, clntSock;
    struct sockaddr_in servAddr, clntAddr;

    pid_t pid;
    struct sigaction act;
    socklen_t addrSz;
    int strLen, state;
    char buf[BUFSIZE];

    if (argc != 2) {
        printf("usage : %s <port>\n", argv[0]);
        exit(1);
    }

    act.sa_handler = readChildProc;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    state = sigaction(SIGCHLD, &act, 0);
    servSock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(atoi(argv[1]));

    if (bind(servSock, (struct sockaddr*)&servAddr, sizeof(servAddr)) == -1)
        errorHandling("bind() error");
    if (listen(servSock, 5) == -1)
        errorHandling("listen() error");

    while(1) {
        addrSz = sizeof(clntAddr);
        clntSock = accept(servSock, (struct sockaddr*)&clntAddr, &addrSz);
        if (clntSock == -1)
            continue;
        else
            puts("new client connected...");

        pid = fork();
        if (pid == -1) {
            close(clntSock);
            continue;
        }

        if (pid == 0) {
            close(servSock);
            while ((strLen = read(clntSock, buf, BUFSIZE)) != 0)
                write(clntSock, buf, strLen);

            close(clntSock);
            puts("client disconnected...");
            return 0;
        }
        else
            close(clntSock);
    }

    close(servSock);
    return 0;
}

void errorHandling(char* msg) {

    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}

void readChildProc(int sig) {

    pid_t pid;
    int status;
    pid = waitpid(-1, &status, WNOHANG);
    printf("removed proc id : %d\n", pid);
}