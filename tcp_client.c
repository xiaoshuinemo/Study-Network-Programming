#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SERV_IP "127.0.0.1"

int main(void)
{
    int sfd, len;
    struct sockaddr_in serv_addr;
    char buf[255]; 
				
	sfd = socket(AF_INET, SOCK_STREAM, 0);
					
	bzero(&serv_addr, sizeof(serv_addr));                       
	serv_addr.sin_family = AF_INET;                             
	inet_pton(AF_INET, "192.168.0.128", &serv_addr.sin_addr.s_addr);    
	serv_addr.sin_port = htons(6666);                      
							
	connect(sfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
										
	while (1) {
		fgets(buf, sizeof(buf), stdin);
		int r = write(sfd, buf, strlen(buf));       
		printf("Write r ======== %d\n", r);
		len = read(sfd, buf, sizeof(buf));
		printf("Read len ========= %d\n", len);
		write(STDOUT_FILENO, buf, len);
    }
										
    close(sfd);
					
    return 0;
}


