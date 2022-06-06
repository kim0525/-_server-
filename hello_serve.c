
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

void error_handling(char *message);	// 함수 선언

int main(int argc, char* argv[])
{
	int serv_sock;
	int clnt_sock;

	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	socklen_t clnt_addr_size;

	char message[] = "Hello World!Hello World!Hello World!Hello World!Hello World!Hello World!Hello World!Hello World!Hello World!Hello World!";

	if(argc != 2)
	{
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}

	serv_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(serv_sock == -1)
		error_handling("socket() error\n");	// 함수 호출

	memset(&serv_addr, 0, sizeof(serv_addr));	// 메모리 초기화
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);	// 0.0.0.0
	serv_addr.sin_port = htons(atoi(argv[1]));

	if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1)
		error_handling("bind() error\n");		// 함수 호출

	if(listen(serv_sock, 5) == -1)
		error_handling("listen() error\n");	// 함수 호출

	clnt_addr_size = sizeof(clnt_addr);
	clnt_sock = accept(serv_sock, (struct sockaddr*) &clnt_addr, &clnt_addr_size);
	if(clnt_sock == -1)
		error_handling("accept() error\n");	// 함수 호출

	write(clnt_sock, message, sizeof(message));

	close(clnt_sock);
	close(serv_sock);

	return 0;
}

void error_handling(char *message)	// 함수 정의
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
