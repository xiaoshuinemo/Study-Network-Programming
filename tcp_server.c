#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/types.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>

#define SERV_PORT 6666

int main()
{
	int lfd, cfd;
	int len;
	char buf[BUFSIZ], clie_IP[128];

	struct sockaddr_in serv_addr, clie_addr;
	socklen_t clie_addr_len;	

	lfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(SERV_PORT);

	bind(lfd, (struct sockaddr *)& serv_addr, sizeof(serv_addr));

	listen(lfd, 2);
	printf("wait client connection\n");
	struct sockaddr_in client_addr;
	socklen_t cli_len = sizeof(client_addr);
	
	int epfd = epoll_create(2000);	//create epoll tree node
	struct epoll_event ev;			//init epoll tree
	ev.events = EPOLLIN;			//set level trigle
	ev.data.fd = lfd;
	epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev);
	struct epoll_event all[2000];
	
	while(1)
	{
		int ret = epoll_wait(epfd, all, sizeof(all)/sizeof(all[0]), -1);
		printf("epoll_wait\n");

		for(int i = 0; i < ret; i++){
			int fd = all[i].data.fd;

			if(fd == lfd){		//a new connection
				int cfd = accept(lfd, (struct sockaddr*)&client_addr, &cli_len);
				if(cfd == -1){
					printf("accept errror");
					exit(1);
				}
				// set cfd nonblock
				int flag = fcntl(cfd, F_GETFL);
				flag |= O_NONBLOCK;
				fcntl(cfd, F_SETFL, flag);

				struct epoll_event temp;
				temp.events = EPOLLIN | EPOLLET;
				temp.data.fd = cfd;
				epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &temp);

				char ip[64] = {0};
				printf("New client IP: %s, Port: %d\n",
						inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, ip, sizeof(ip)),
						ntohs(client_addr.sin_port));
			}
			else{	//message from old client
				if(!all[i].events & EPOLLIN)
					continue;

				char buf[20] = {0};
				int len;
				while((len = recv(fd, buf, sizeof(buf), 0)) > 0){
					write(STDOUT_FILENO, buf, len);	//print
					send(fd, buf, len, 0);			//send to client
				}
				if(len == 0){
					printf("client disconnect\n");
					ret = epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
					if(ret == -1){
						printf("epoll_ctl del error");
						exit(1);
					}
					close(fd);
				}			
				else if(len == -1){
					if(errno == EAGAIN){
						printf("read success\n");	
					}
					else{
						printf("recv errno\n");
						exit(1);
					}
				}
			
			}
		
		}

//		len = read(cfd, buf, sizeof(buf));
//		write(STDOUT_FILENO, buf, len);	
//		write(cfd, buf, len);
	}
	close(lfd);
	return 0;
}
