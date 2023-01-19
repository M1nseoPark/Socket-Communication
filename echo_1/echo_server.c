#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
void error_handling(char* message);

int main(int argc, char* argv[]) {

    int serv_sock, clnt_sock;
    char message[BUF_SIZE];
    int str_len, i;

    struct sockaddr_in serv_adr, clnt_adr;
    socklen_t clnt_adr_sz;

    if (argc != 2) {
        printf("Usage : %s<port>\n", argv[0]);
        exit(1);
    }

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);   // TCP 소켓 생성
    if (serv_sock == -1)   // 소켓 생성 실패 시 -1 반환 
        error_handling("socket() error");

    memset(&serv_adr, 0, sizeof(serv_adr));   // 구조체 변수 serv_adr의 모든 멤버 0으로 초기화 
    serv_adr.sin_family = AF_INET;   // 주소체계 지정 
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);   // IP 주소 초기화
    serv_adr.sin_port = htons(atoi(argv[1]));   // PORT 번호 초기화

    if (bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)   // 주소할당 실패 시 -1 반환
        error_handling("bind() error");

    if (listen(serv_sock, 5) == -1)   // 소켓을 연결 요청을 받을 수 있는 상태로 변경 
        error_handling("listen() error");

    clnt_adr_sz = sizeof(clnt_adr);
    
    // accept 함수가 5번 호출되어 총 5개의 클라이언트에게 순서대로 에코 서비스를 제공함
    // accept 함수는 클라이언트 소켓의 연결 요청을 수락하는 함수 
    for (i = 0 ; i < 5 ; i++) {
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
        if (clnt_sock == -1)
            error_handling("accept() error");
        else
            printf("connected %d\n", i+1);
        
        // 실제 에코 서비스가 이루어지는 부분 -> 읽어들인 문자열 그대로 전송
        while ((str_len = read(clnt_sock, message, BUF_SIZE)) != 0)
            write(clnt_sock, message, str_len);
        
        close(clnt_sock);   
    }

    close(serv_sock);
    return 0;
}

void error_handling(char* message) {

    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
