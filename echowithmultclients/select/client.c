#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    
    // Create a socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    // Initialize server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(9999);
    
    // Convert IP address from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr)<=0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }
    
    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }
    
    printf("Connected to server.\n");
    
    while(1) {
        printf("Enter message: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        
        // Send the message to server
        send(client_socket, buffer, strlen(buffer), 0);
        
        // Receive the echo message from server
        int valread = read(client_socket, buffer, BUFFER_SIZE);
        buffer[valread] = '\0';
        
        printf("Server echo: %s\n", buffer);
    }
    
    // Close the socket
    close(client_socket);
    
    return 0;
}

