#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>

time_t t; 
struct tm *timeinfo;
char timebuffer[100];

#define MAX_BUFFER_SIZE 4096   // maximum size of the buffer
#define PORT 8080           

int main() 
{
    
    int server_socket, client_socket;  // we have 2 sockets in server. client_socket is used as a socket for client. server_socket is used for listening.
    struct sockaddr_in server_addr, client_addr;  // these structure variables are used to store the addresses of server and client side. (IPV4)
    socklen_t client_addr_len = sizeof(client_addr);  
    socklen_t server_addr_len = sizeof(server_addr); 
    char buffer[MAX_BUFFER_SIZE];  // buffer to store the input from the user
    
    server_socket = socket(AF_INET, SOCK_STREAM, 0);   // Create socket 
    if (server_socket == -1) 
    {
        perror("Error creating socket");
        exit(1);
    }

    
    // Set up server address structure
    server_addr.sin_family = AF_INET;  // For IPV4
    server_addr.sin_addr.s_addr = INADDR_ANY; // Server side is open for any clients to interact with.
    server_addr.sin_port = htons(PORT);    // converts the port number to BIG ENDIAN.

    
    if (bind(server_socket, (struct sockaddr*)&server_addr, server_addr_len) == -1)  // Binds the socket with the server address
    {
        perror("Error binding socket");
        exit(1);
    }

    // Listen for incoming connections
    if (listen(server_socket, 1) == -1)   // Until now only one client per server
    {
        perror("Error listening for connections");
        exit(1);
    }

    printf("Server listening on port %d...\n", PORT);

    // Accepts a connection
    client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);  // we pass the client address
    if (client_socket == -1) 
    {
        perror("Error accepting connection");
        exit(EXIT_FAILURE);
    }

    // Receive and echo back data
    while (1) 
    {
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);   // receives the input buffer from the client side
        if (bytes_received < 1) 
            break;    // Connection closed or error
            
        printf("Received on Server Side: ");
        printf("%s\n", buffer);
        
    	time(&t);
        timeinfo = localtime(&t);
        strftime(timebuffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
        strcat(buffer, " ");
      	strcat(buffer, timebuffer);
    	
        printf("Now echoing Back to the server\n");
        send(client_socket, buffer, bytes_received + 25, 0);  // Echo back the received data
    }

    // Close sockets
    close(client_socket);
    close(server_socket);

    return 0;
}

