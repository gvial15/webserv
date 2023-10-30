#include <cstring>
#include <iostream>
#include <unistd.h>  // for close()
#include <arpa/inet.h>  // for inet_ntop()
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>  // for poll()

#define PORT 8080
#define MAX_CLIENTS 10  // Maximum number of clients

int main()
{
//  ***************** this happens in the Server class *****************
    int server_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    struct sockaddr_in sender_address; // to get the client ip/infos
    char buffer[1024] = {0};

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // setup sockaddr_in
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Binding the socket to the port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listening for a connection
    if (listen(server_fd, 3) < 0)
    {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

//  ***************** Webserv run() *****************
    struct pollfd fds[MAX_CLIENTS + 1];  // +1 for the server socket
    memset(fds, 0, sizeof(fds));
    fds[0].fd = server_fd;
    fds[0].events = POLLIN;

    while (true)
    {
        int ret = poll(fds, MAX_CLIENTS + 1, -1);  // -1 for infinite timeout
        if (ret < 0)
        {
            perror("poll failed");
            exit(EXIT_FAILURE);
        }

        if (fds[0].revents & POLLIN) // New connection on server socket
        {
            int new_socket;
            if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0)
            {
                perror("accept failed");
                exit(EXIT_FAILURE);
            }

            // show new client id
            socklen_t sender_addrlen = sizeof(sender_address);
            if (getpeername(new_socket, (struct sockaddr*)&sender_address, &sender_addrlen) == 0)
            {
                char client_ip[INET_ADDRSTRLEN];
                if (inet_ntop(AF_INET, &sender_address.sin_addr, client_ip, INET_ADDRSTRLEN))
                {
                    std::cout << "Connection from: " << client_ip << std::endl;
                }
                else 
                {
                    perror("inet_ntop failed");
                }
            }
            else 
                perror("getpeername failed");

            for (int i = 1; i <= MAX_CLIENTS; ++i)
            {
                if (fds[i].fd == 0) {
                    fds[i].fd = new_socket;
                    fds[i].events = POLLIN;
                    break;
                }
            }
        }

        for (int i = 1; i <= MAX_CLIENTS; ++i) // Check all client sockets
        {
            if (fds[i].revents & POLLIN)
            {
                int bytes_read = read(fds[i].fd, buffer, 1024);
                if (bytes_read <= 0) // Connection closed or error
                {
                    close(fds[i].fd);
                    fds[i].fd = 0;  // Mark as available
                }
                else
                {
                    std::cout << buffer << std::endl;
                    memset(buffer, 0, strlen(buffer));
                }
            }
        }
    }

    close(server_fd);
    return 0;
}


// #include <poll.h>
// #include <cstring>
// #include <string>
// #include <iostream>
// #include <unistd.h>  // for close()
// #include <arpa/inet.h>  // for inet_ntop()
// #include <sys/socket.h>
// #include <netinet/in.h>

// #define PORT 8080

// int main() {
//     int server_fd, receiving_socket;
//     struct sockaddr_in address;
// 	struct sockaddr_in sender_address; // to get the client ip/infos
//     char buffer[1024] = {0};

//     // Creating socket file descriptor
//     if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
//         perror("socket failed");
//         exit(EXIT_FAILURE);
//     }

//     // setup sockaddr_in
//     address.sin_family = AF_INET;
//     address.sin_addr.s_addr = INADDR_ANY;
//     address.sin_port = htons(PORT);


//     // Binding the socket to the port
//     if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
//         perror("bind failed");
//         exit(EXIT_FAILURE);
//     }

//     // Listening for a connection
//     if (listen(server_fd, 3) < 0) {
//         perror("listen failed");
//         exit(EXIT_FAILURE);
//     }

// 	// Accepting an incoming connection
// 	if ((receiving_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0)
// 	{
// 		perror("accept failed");
// 		exit(EXIT_FAILURE);
// 	}

    // socklen_t sender_addrlen = sizeof(sender_address);
    // if (getpeername(receiving_socket, (struct sockaddr*)&sender_address, &sender_addrlen) == 0)
    // {
    //     char client_ip[INET_ADDRSTRLEN];
    //     if (inet_ntop(AF_INET, &sender_address.sin_addr, client_ip, INET_ADDRSTRLEN))
    //     {
    //         std::cout << "Connection from: " << client_ip << std::endl;
    //     }
    //     else 
    //     {
    //         perror("inet_ntop failed");
    //     }
    // }
    // else 
    //     perror("getpeername failed");

// 	// Reading data from the socket
// 	while (buffer[0] != '0')
// 	{
// 		read(receiving_socket, buffer, 1024);
// 		std::cout << buffer << std::endl;
// 		memset(buffer, 0, strlen(buffer));
// 	}
//     close(server_fd);
// 	close(receiving_socket);

//     return 0;
// }