#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define SERVER_PORT 8081  // This port number is reserved for web servers
#define MAXLINE 4096  // maximum length of the buffer

void handle_error(const char *msg)  // FUnction to handle error calls
{
	perror(msg);  // prints the error message in stderr
    	exit(1);
}

int main() 
{
	int server_fd, client_fd;  // we require 2 sockets in the server, one is for listening and the other is for accepting the connections
    	struct sockaddr_in server_addr, client_addr; 
    	socklen_t client_len = sizeof(client_addr); 
    	char buffer[1024];  // Buffer to store the data

    
    	server_fd = socket(AF_INET, SOCK_STREAM, 0);  // Create socket for server
    	if (server_fd == -1) 
        	handle_error("Unable to create socket");

    	// Bind to address
    	server_addr.sin_family = AF_INET;
    	server_addr.sin_addr.s_addr = INADDR_ANY;
    	server_addr.sin_port = htons(SERVER_PORT);
    
    
    	int reuse = 1;
    	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1)// This is used to bind the server to that port only. (avoids bind error)
    		handle_error("Unable to set socket options");

    	if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) 
        	handle_error("Unable to bind");

    	if (listen(server_fd, 1) == -1) // ready to accept client connections requests.
        	handle_error("Unable to listen");
    

    	printf("Server started, listening on port %d...\n", SERVER_PORT);

    	while (1) 
    	{
        	// Accept incoming connection
        	client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        	if (client_fd == -1) 
          		handle_error("Unable to accept");
        	

        // Read HTTP request from the client
        ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
        if (bytes_received == -1) 
            handle_error("Unable to receive data");
        
        // Prepare HTTP response to the client
        const char *response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "\r\n"
            "<html>\
             	<head>HTTP WEB SERVER</head>\
             		<body>\
             			<h3>Success, Lets gooooo</h3>\
             			<p>This is the message sent from the server side indication that this program worked</p>\
             		</body>\
             </html>";

        // Send response
        ssize_t bytes_sent = send(client_fd, response, strlen(response), 0);
        if (bytes_sent == -1) 
        	handle_error("Unable to send data");
        	
        // Close connection
        close(client_fd);
    	}

    	// Close socket
    	close(server_fd);

    	return 0;
}

