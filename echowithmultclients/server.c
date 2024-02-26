/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define MAX_CLIENTS 5
#define BUFFER_SIZE 1024

void *handle_client(void *arg)  // handle client calls
{
    int client_socket = *((int *)arg);
    char buffer[BUFFER_SIZE];
    int bytes_received;

    while ((bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0)  // Loop to receive and echo messages back to client
    {
        send(client_socket, buffer, bytes_received, 0);
        memset(buffer, 0, sizeof(buffer));
    }

    // Close client socket
    close(client_socket);

    return NULL;
}

int main() 
{
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    pthread_t tid[MAX_CLIENTS]; //standard API for creating and manipulating threads in a multi-threaded application.

    int client_count = 0;  // to keep count of clients connected to the server

    // Create server socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) 
    {
        perror("Socket creation failed");
        exit(1);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;  // For IPV4
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);  // converting the port number to network byte order

    // Bind server socket to server address
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Socket binding failed");
        exit(1);
    }

    // Start listening for incoming connections
    if (listen(server_socket, MAX_CLIENTS) < 0) 
    {
        perror("Socket listening failed");
        exit(1);
    }

    printf("Server listening on port %d...\n", PORT);

    // Accept and handle incoming connections
    while (1) 
    {
        // Accept incoming connection
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_socket < 0) 
        {
            perror("Error accepting connection");
            continue;
        }

        printf("New client connected\n");
        printf("Number of clients connected: %d\n", client_count);

        // Create thread to handle client
        if (pthread_create(&tid[client_count], NULL, handle_client, &client_socket) != 0)    //creating a new thread to connect the server to the client
        {
            perror("Thread creation failed");
            close(client_socket);
        }

        // Limit the number of clients
        if (client_count++ >= MAX_CLIENTS) 
        {
            printf("Maximum number of clients reached. No longer accepting connections.\n");
            break;
        }
    }

    // Join threads
    for (int i = 0; i < client_count; ++i) 
    {
        pthread_join(tid[i], NULL);
    }

    // Close server socket
    close(server_socket);

    return 0;
} */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8081
#define MAX_CLIENTS 5
#define BUFFER_SIZE 1024

void *handle_client(void *arg)  // handle client calls
{
    int client_socket = *((int *)arg);
    char buffer[BUFFER_SIZE];
    int bytes_received;

    while ((bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0)  // Loop to receive and echo messages back to client
    {
        printf("Message received in server: %s\n", buffer);
        printf("Message Echoed back to the client\n");
        send(client_socket, buffer, bytes_received, 0);
        memset(buffer, 0, sizeof(buffer));
    }

    // Close client socket
    close(client_socket);

    return NULL;
}

int main() 
{
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    pthread_t tid[MAX_CLIENTS]; //standard API for creating and manipulating threads in a multi-threaded application.

    int client_count = 0;  // to keep count of clients connected to the server

    // Create server socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) 
    {
        perror("Socket creation failed");
        exit(1);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;  // For IPV4
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);  // converting the port number to network byte order

    // Bind server socket to server address
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Socket binding failed");
        exit(1);
    }

    // Start listening for incoming connections
    if (listen(server_socket, MAX_CLIENTS) < 0) 
    {
        perror("Socket listening failed");
        exit(1);
    }

    printf("Server listening on port %d...\n", PORT);

    // Accept and handle incoming connections
    while (1) 
    {
        // Accept incoming connection
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_socket < 0) 
        {
            perror("Error accepting connection");
            continue;
        }

        printf("New client connected\n");
        printf("Number of clients connected: %d\n", client_count + 1);

        // Create thread to handle client
        if (pthread_create(&tid[client_count], NULL, handle_client, &client_socket) != 0)    //creating a new thread to connect the server to the client
        {
            perror("Thread creation failed");
            close(client_socket);
        }

        // Limit the number of clients
        if (client_count++ >= MAX_CLIENTS) 
        {
            printf("Maximum number of clients reached. No longer accepting connections.\n");
            break;
        }
    }

    // Join threads
    for (int i = 0; i < client_count; ++i) 
    {
        pthread_join(tid[i], NULL);
    }

    // Close server socket
    close(server_socket);

    return 0;
}

