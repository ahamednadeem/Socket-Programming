//Implemented POST method in HTTP webs server

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

#define PORT 8080
#define BACKLOG 20
#define MAX_SIZE 1024

void handle_post_request(int sockfd, const char *body)   
{
    FILE *file = fopen("post_data.txt", "a");  // we open this file to store the body of the post request
    if (file == NULL) 
    {
        perror("Failed to open file");
        exit(1);
    }
    
    fprintf(file, "%s\n", body);  // we print the body of the request in the above file
    fclose(file);

    const char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n\
    <html>\
    	<body>\
    		<h1>POST Request Received and Stored</h1>\
    		<h2>Ahamed Nadeem</h2>\
    	</body>\
    </html>";
    
    send(sockfd, response, strlen(response), 0);  
}

void handle_client(int sockfd)   // this function receives the input request and gets the body of the request
{
    char buffer[MAX_SIZE];  // buffer to store 
    int n;

    if ((n = recv(sockfd, buffer, MAX_SIZE - 1, 0)) < 0)   // receives the input request
    {
        perror("Error in receiving request");
        exit(1);
    }
    buffer[n] = '\0'; 
    printf("buffer: %s\n", buffer);

    char *body = strstr(buffer, "\r\n\r\n"); // now character pointer body points to the starting location of the string "\r\n\r\n"  
    
    printf("Message Received: %s\n", buffer);
    if (body != NULL) 
    {
        body += 4; // Move past the "\r\n\r\n" to get the body of the request
        handle_post_request(sockfd, body);  // aftr we get the body of the request we call the handle_post_request 
    }

    close(sockfd); // close the socket
}

int main() 
{
    int sockfd, newsockfd;  // server and client socket descriptors
    struct sockaddr_in serv_addr;
    socklen_t addr_len = sizeof(serv_addr);

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)   // create socket for server
    {
        perror("failed to create server socket");
        exit(1);
    }

    int yes = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &yes, sizeof(yes)))   // to overcome bind error
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    memset(&serv_addr, '\0', sizeof(serv_addr));   // resets the buffer
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)   // bind the socket to address
    {
        perror("bind error");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) < 0)  // listening to incoming connections
    {
        perror("server failed to listen");
        exit(1);
    }

    printf("Server listening on port %d...\n", PORT);
    printf("Send POST request using curl command...\n");
    

    while (1)  // infinite loop to accept connections 
    {
        if ((newsockfd = accept(sockfd, (struct sockaddr *)&serv_addr, &addr_len)) < 0)   // accepts new connections
        {
            perror("server: accept");
            exit(1);
        }

        handle_client(newsockfd); 
    }

    return 0;
}
