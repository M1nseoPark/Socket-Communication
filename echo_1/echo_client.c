#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
void error_handling(char* message);

int main(int argc, char* argv[]) {
    
    int sock;
    char message[BUF_SIZE];
    int str_len;
    struct sockaddr_in serv_adr;

    if (argc != 3) {
        printf("Usage : %s<IP> <port>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        error_handling("socket() error");

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_adr.sin_port = htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == 1)
        error_handling("connect() error");
    else
        puts("connected.......");

    while(1) {
        fputs("Input message(Q to quit): ", stdout);
        fgets(message, BUF_SIZE, stdin);
        
        // strcmp는 인자로 주어진 두 문자열의 일치 여부를 확인하는 함수 
        if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
            break;
        
        // **문제점** read, write 함수가 호출될 때마다 문자열 단위로 실제 입출력이 이루어진다는 잘못된 가정을 하고 있음 
        // 둘 이상의 write 함수호출로 전달된 문자열 정보가 묶여서 한번에 서버로 전송될 수 있음 -> 클라이언트는 바로 이전에 전송한 문자열까지 받게 됨 
        // 문자열이 길어서 여러 개의 패킷으로 나누어 보내는 경우, 데이터의 모든 조각이 클라이언트에 전송되지 않았을 때 read 함수를 호출할 수도 있음
        write(sock, message, strlen(message));
        str_len = read(sock, message, BUF_SIZE-1);
        message[str_len] = 0;
        printf("Message from server:%s", message);
    }

    close(sock);
    return 0;
}

void error_handling(char* message) {
    
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
