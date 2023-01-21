#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
#define RLT_SIZE 4   // 연산 결과의 바이트 수
#define OPSZ 4   // 피연산자의 바이트 수 
void error_handling(char* message);

int main(int argc, char* argv[]) {

    int sock;
    char opmsg[BUF_SIZE];   // 데이터를 누적해서 송수신해야 하기 때문에 배열을 기반으로 생성하는 게 좋음 
    int result, opnd_cnt, i;
    struct sockaddr_in serv_adr;

    if(argc != 3) {
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

    if (connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("connect() error!");
    else
        puts("connected......");

    fputs("Operand count: ", stdout);
    scanf("%d", &opnd_cnt);   // 피연산자의 개수 정보 입력 받음 
    opmsg[0] = (char)opnd_cnt;   // 입력 받은 개수 정보를 opmsg 배열에 저장함(1바이트)

    for (i = 0; i < opnd_cnt; i++) {
        printf("Operand %d: ", i+1);
        scanf("%d", (int*)&opmsg[i*OPSZ+1]);
    }
    
    fgetc(stdin);   // 앞서 버퍼에 남아있는 \n 문자의 삭제 위함 
    fputs("Operator: ", stdout);
    scanf("%c", &opmsg[opnd_cnt*OPSZ+1]);   // 연산자 정보르 입력받아 opmsg 배열에 저장함 
    
    write(sock, opmsg, opnd_cnt*OPSZ+2);   // opmsg에 저장되어 있는 연산 관련 정보를 전송함 
    read(sock, &result, RLT_SIZE);   // 서버가 전송한 연산 결과 저장 

    printf("Operation result: %d \n", result);
    close(sock);
    return 0;
}

void error_handling(char* message) {

    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
