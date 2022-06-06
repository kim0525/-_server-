#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define BUF_SIZE 100
#define MAX_CLNT 256	// 클라이언트의 최대 수 

void * handle_clnt(void * arg);		// 쓰레드 함수 
void send_msg(char * msg, int len);	// 통신으로 데이터를 수신하면 지금 연결된 클라이언트 소켓 들에게 메세지 전달 
void error_handling(char * msg);	// 에러 처리 함수 

int clnt_cnt=0;					// 접속된 클라이언트의 수 
int clnt_socks[MAX_CLNT];		// 클라이언트들과 접속된 소켓을 관리하기 위한 배열 
pthread_mutex_t mutx;			// 클라이언트들의 추가.삭제시 임계영역을 만들어 주기 위해 사용 

int main(int argc, char *argv[])
{
	// 변수 선언 
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	int clnt_adr_sz;
	
	pthread_t t_id;
	
	// 인자들의 개수를 확인 
	if(argc!=2) {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
  
	pthread_mutex_init(&mutx, NULL);
	
	// 서버 소켓 생성 및 설정 
	serv_sock=socket(PF_INET, SOCK_STREAM, 0);

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET; 
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_adr.sin_port=htons(atoi(argv[1]));
	
	if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr))==-1)
		error_handling("bind() error");
	if(listen(serv_sock, 5)==-1)
		error_handling("listen() error");
	
	// 연결 요청시 처리하는 부분 
	while(1)
	{
		clnt_adr_sz=sizeof(clnt_adr);
		clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr,&clnt_adr_sz);
		
		pthread_mutex_lock(&mutx);
		clnt_socks[clnt_cnt]=clnt_sock;	// 임계영역 
		clnt_cnt++;
		pthread_mutex_unlock(&mutx);
	
		pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sock);
		pthread_detach(t_id);
		
		printf("Connected client IP: %s \n", inet_ntoa(clnt_adr.sin_addr));
	}
	close(serv_sock);
	return 0;
}
	
void * handle_clnt(void * arg)
{
	int clnt_sock=*((int*)arg);
	int str_len=0, i;
	char msg[BUF_SIZE];
	
	while((str_len=read(clnt_sock, msg, sizeof(msg))) != 0)		// 반복, EOF를 전달 받을 때까지
		send_msg(msg, str_len);
	
	// remove disconnected client
	pthread_mutex_lock(&mutx);
	for(i=0; i<clnt_cnt; i++)   
	{
		if(clnt_sock==clnt_socks[i])
		{
			printf("삭제된 클라이언트 : %d\n", clnt_sock);
			while(i++<clnt_cnt-1)
				clnt_socks[i]=clnt_socks[i+1];
			break;
		}
	}
	clnt_cnt--;
	pthread_mutex_unlock(&mutx);
	
	close(clnt_sock);
	return NULL;
}

void send_msg(char * msg, int len)   // 모든 접속자에게 메세지를 전달 
{
	int i;
	
	pthread_mutex_lock(&mutx);
	for(i=0; i<clnt_cnt; i++)
		write(clnt_socks[i], msg, len);
	pthread_mutex_unlock(&mutx);
	
}
void error_handling(char * msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
