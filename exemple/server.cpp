#include <cstring>
#include <iostream>

#include <unistd.h> // read & write
#include <fcntl.h>

#include <sys/socket.h> // socket - bind
#include <arpa/inet.h> // inet_addr

// server parameters
#define SERV_IP		"127.0.0.1"
#define SERV_PORT	8080

int main()
{
    int socket_fd, receive_fd;
    unsigned int len; // for use in accept()
    struct sockaddr_in server, client;
	int html_index_len;
	int html_index_fd;
    char html_index[1053] = {0};
	int  len_receive, len_send = 0;
	char response[1000] = "\nHTTP/1.1 200 Ok\nContent-Type: text/html\nContent-Length: 700\n\n";
	char buff_receive[1000] = {0};

    html_index_fd = open("index.html", O_RDONLY);
    html_index_len = read(html_index_fd, html_index, 1053);

	// create socket
	if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    	return (printf("socket() error\n"), -1);
	// setup server (struct sockddr_in)
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(SERV_IP);
    server.sin_port = htons(SERV_PORT);
	// bind() socket to server (struct sockddr_in)
    if ((bind(socket_fd, (struct sockaddr *) &server, sizeof(server))) != 0)
        return (printf("bind() error\n"), -1);
    else   
        std::cout << "socket is binded!\n";
    // listen() to incoming connection on the socket
    if ((listen(socket_fd, 5)) != 0)
        printf("listen() error\n");
    else
        printf("server listen to port %i\n", ntohs(server.sin_port));
    while (1)
	{
        std::cout << " +++++++ Waiting for new connection ++++++++ \n";
		// accept()
        receive_fd = accept(socket_fd, (struct sockaddr *) &client, &len);
        if (receive_fd == -1)
            std::cout << "error accept\n";
        else
		{
            printf("connection received\n");
            std::cout << "client addr is " << inet_ntoa(client.sin_addr) << std::endl;
        }
        len_receive = read(receive_fd, buff_receive, sizeof(buff_receive));
        std::cout << "received request: \n" << buff_receive; 
        if (len_receive < 0)
            std::cout << "nothing received!\n";
        write(receive_fd, response, sizeof(response));
        close(receive_fd);
	}
    return (0);
}