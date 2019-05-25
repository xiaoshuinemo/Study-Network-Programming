#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/types.h>
#include <ctype.h>
#include <arpa/inet.h>

#define SERV_PORT 6666

int main()
{
	int sfd, cfd;
	int len;
	char buf[BUFSIZ], clie_IP[128];

	struct sockaddr_in serv_addr, clie_addr;
	socklen_t clie_addr_len;	

	sfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(SERV_PORT);

	bind(sfd, (struct sockaddr *)& serv_addr, sizeof(serv_addr));

	listen(sfd, 2);
	printf("wait client connection");

	clie_addr_len = sizeof(clie_addr_len);
	cfd = accept(sfd, (struct sockaddr *)&clie_addr, &clie_addr_len);
	printf("cfd = ---%d\n", cfd);
	
	//printf("client ip: %s port : %d\n", inet_ntop(AF_INET, &clie_addr.sin_addr.s_addr, client_IP, sizeof(clie_IP)), ntohs(clie_addr.sin_port));

	while(1)
	{
		len = read(cfd, buf, sizeof(buf));
		write(STDOUT_FILENO, buf, len);	
		write(cfd, buf, len);
	}
	close(sfd);
	close(cfd);
	return 0;
}
