// client side program to send message to the server

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    // Create a socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    {
        perror("Socket creation error");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);

  
    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0)    // Convert IPv4 addresses from text to binary form
    {
        perror("Invalid address/ Address not supported");
        return -1;
    }

    
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)  // Connect to the server
    {
        perror("Connection Failed");
        return -1;
    }

    
    while (1)  // Send and receive messages to server 
    {
        char buffer[BUFFER_SIZE];   // buffer to store the the message
        
        printf("Enter the Message  to be sent to the server: \t");
        fgets(buffer, sizeof(buffer), stdin); // get input from stdin and store it in the buffer
        
        printf("Message sent to server: %s\n", buffer);
        send(sock, buffer, strlen(buffer), 0);
        
        
        int valread = read(sock, buffer, BUFFER_SIZE);  // Receive message from server
        if (valread < 0) 
        {
            perror("Read failed");
            return -1;
        }
        buffer[valread] = '\0'; // Ensure null-termination of received message
        printf("Message received from server: %s\n", buffer);

        sleep(1); // Wait for 1 second before sending the next message
    }

    close(sock);

    return 0;
}

