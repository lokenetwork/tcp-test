#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include "p_t_p.h"

#define ERR_EXIT(m) \
				do{\
					perror(m);\
					exit(EXIT_FAILURE);\
				}while(0)
void handler(int sig){
  printf("receive the server signal,exit\n");
	exit(EXIT_SUCCESS); 
}

int main(void){
	int listenfd;
	int flag=1,len=sizeof(int); 
	if( (listenfd = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP)) < 0 ){
		ERR_EXIT("socket");
	}

	struct sockaddr_in servaddr;
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(5188);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if(setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&flag,len) == -1){
		ERR_EXIT("setsockopt");
	}

	if( bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0 ){
		ERR_EXIT("bind");
	}
	if( listen(listenfd,SOMAXCONN) < 0 ){
		ERR_EXIT("listen");
	}
	struct sockaddr_in peeraddr;
	socklen_t peerlen = sizeof(peeraddr);
	int conn;
	pid_t pid;
	while(1){
		if( (conn = accept(listenfd, (struct sockaddr*)&peeraddr, &peerlen)) < 0 ){
			ERR_EXIT("accept");
		}
		printf("ip=%s port=%d\n",inet_ntoa(peeraddr.sin_addr),ntohs(peeraddr.sin_port));
		pid = fork();
		if( pid == -1 ){
			ERR_EXIT("fork");
		}
		//父进程处理接受信息
		if( pid != 0 ){
			//如果已连接，关闭监听
			close(listenfd);
  		    char recvbuf[1024];
			//死循环接受信息
			while(1){
                memset(recvbuf,0,sizeof(recvbuf));
                int ret = read(conn, recvbuf, sizeof(recvbuf));
                //判断客户端是否断开
                if( ret == 0 ){
                    printf("Client close connection\n");
                    //发送信号通知子进程,0是通知所有子进程
                    kill(0,SIGUSR1);
                    //kill(0,65);
                    break;
                }
                //传输错误
                if( ret == -1 ){
                    printf("Message translation error\n");
                }
                char recvbuf2[1024] = "get data";
                fputs(recvbuf2,stdout);
			}
		}else{
			//等待接受父进程的信号
			signal(SIGUSR1,handler);
			//signal(65,handler);
			//子进程处理发送信息
			char sendbuf[1024] = {0};
			//死循环获取键盘输入信息发送
			while(1){
				while( fgets(sendbuf, sizeof(sendbuf),stdin) != NULL ){ 
   		  	write(conn,sendbuf,strlen(sendbuf));
    			memset(sendbuf,0,sizeof(sendbuf));
  			}
			}
			//跳出while循环就关闭连接
			close(conn);
		}
		break;
	}

	return 0;

}



