#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8081
#define BUFFER_SIZE 1024

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // Create client socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);  
    if (client_socket < 0) 
    {
        perror("Socket creation failed");
        exit(1);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(PORT);

    // Connect client to server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection to server failed");
        exit(1);
    }

    printf("Connected to server\n");

    // Send and receive messages
    while (1) 
    {
        printf("Enter message: ");
        fgets(buffer, BUFFER_SIZE, stdin);  // get input message from the user to send data to the server

        // Send message to server
        send(client_socket, buffer, strlen(buffer), 0);
        printf("Message sent to the server from client 1\n");

        // Receive message from server
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) 
        {
            printf("Server Disconnected\n");
            break;
        }
        printf("Server response: %s\n", buffer);
    }

    // Close client socket
    close(client_socket);

    return 0;
}

