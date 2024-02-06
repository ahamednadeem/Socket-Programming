#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>


#define MAX_BUFFER_SIZE 4096
#define SERVER_IP "127.0.0.1"  // IP address of the local host (server)
#define PORT 8080

time_t t; 
struct tm *timeinfo;
char timebuffer[100];
 
int main() 
{
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[MAX_BUFFER_SIZE];

    // Create socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) 
    {
        perror("Error creating socket");
        exit(0);
    }
    

    // Set up server address structure
    server_addr.sin_family = AF_INET;  // For IPV4
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);  // converts the string IP address to network byte order
    server_addr.sin_port = htons(PORT);  // converts the port number to BIG ENDIAN.

    // Connect to the server
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) 
    {
        printf("Error connecting to server");  
        exit(0);
    }

    printf("Success, Connected to server on %s:%d\n", SERVER_IP, PORT);

    // Send and receive data
    while (1) 
    {
        printf("Enter message to send to the server (or 'exit' to quit): ");
        fgets(buffer, sizeof(buffer), stdin);  // get the input string from the user and store it in the buffer

	if (strcmp(buffer, "exit\n") == 0)     // Exit if the user enters 'exit'
            break;
        else
        {
        	time(&t);
        	timeinfo = localtime(&t);
       	strftime(timebuffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
       	strcat(buffer, " ");
      		strcat(buffer, timebuffer);
        	printf("Sending from Client Side: \n");
        	printf("%s\n", buffer);
        	sleep(2);
        }
        

        
        send(client_socket, buffer, strlen(buffer), 0); // Send the message to the server

        ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer), 0); // Receive and print the echoed data from the server
        if (bytes_received < 1) 
            break;    // Connection closed or error
        sleep(2);
        printf("Received Echoed text from the server: \n");
        time(&t);
        timeinfo = localtime(&t);
        strftime(timebuffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
        strcat(buffer, " ");
      	strcat(buffer, timebuffer);
        printf("%s\n", buffer);
      
      
    }

    // Close the socket
    close(client_socket);

    return 0;
}

