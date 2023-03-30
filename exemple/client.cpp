#include <iostream>

#include <unistd.h> // read & write

#include <sys/socket.h> // socket - bind
#include <arpa/inet.h> // inet_addr

// server parameters
#define SERV_ADDR   "10.13.1.5"
#define SERV_PORT       8080

char buff_send[100] = "Hello server, I am client\n";
char buff_receive[100];

int main() {

    int sfd;
    struct sockaddr_in servaddr;

    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1) {
        std::cout << "socket() error\n";
    	return (-1);
	}
    else
		std::cout << "socket fd = " << sfd << std::endl;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(SERV_ADDR);
    servaddr.sin_port = htons(SERV_PORT);
    if (connect(sfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) != 0)
	{
        std::cout << "connect() error\n";
        return (0);
    }
    std::cout << "Connected to server\n";
    // send msg to server
    write(sfd, buff_send, sizeof(buff_send));
    read(sfd, buff_receive, sizeof(buff_receive));
	printf("%s\n", buff_receive);
    close(sfd);
}
