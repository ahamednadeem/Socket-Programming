#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <poll.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024
#define SERVER_PORT 8080

int main() {
    int server_fd, new_socket, addrlen, activity, i;
    int client_sockets[MAX_CLIENTS] = {0};
    struct sockaddr_in address;
    char buffer[BUFFER_SIZE];

    // Create server socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options to allow multiple connections
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("Setsockopt failed");
        exit(EXIT_FAILURE);
    }

    // Bind server socket to localhost and a port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(SERVER_PORT);
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    struct pollfd fds[MAX_CLIENTS + 1]; // Plus 1 for the server socket
    fds[0].fd = server_fd;
    fds[0].events = POLLIN;

    printf("Waiting for connections...\n");

    while (1) {
        // Call poll to wait for events on the sockets
        if (poll(fds, MAX_CLIENTS + 1, -1) < 0) {
            perror("Poll failed");
            exit(EXIT_FAILURE);
        }

        // Check if there is a new incoming connection
        if (fds[0].revents & POLLIN) {
            if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
                perror("Accept failed");
                exit(EXIT_FAILURE);
            }
            printf("New connection, socket fd is %d, IP is: %s, port: %d\n", new_socket,
                   inet_ntoa(address.sin_addr), ntohs(address.sin_port));

            // Add the new socket to the list of client sockets
            for (i = 1; i < MAX_CLIENTS + 1; i++) {
                if (client_sockets[i] == 0) {
                    client_sockets[i] = new_socket;
                    break;
                }
            }

            // Add the new socket to the pollfd structure
            fds[i].fd = new_socket;
            fds[i].events = POLLIN;
        }
	int n;
        // Check for data from clients
        for (i = 1; i < MAX_CLIENTS + 1; i++) 
        {
        
            if (fds[i].revents & POLLIN) {
            	memset(&buffer,'\0',strlen(buffer));
                if ((n = recv(fds[i].fd, buffer, BUFFER_SIZE, 0)) <= 0) {
                    close(fds[i].fd);
                    client_sockets[i] = 0;
                } else {
                    printf("Received message from client %d: %s\n", i, buffer);
                    // Send back the received message only to the client who sent it
                    send(fds[i].fd, buffer, strlen(buffer), 0);
                }
            }
        }
    }

    return 0;
}

