// Multiple clients ECHO SERVER using POLL

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <poll.h>
#include<time.h>

#define PORT 8080
#define MAX_CLIENTS 10  // maximum clients that can be connected

int main() 
{
	int server_fd, new_socket, valread;
    	struct sockaddr_in server;
    	char buffer[1024] = {0};  // buffer is stored with 0's

    
    	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)  // A server socket is created
    	{
        	perror("socket failed");
        	exit(1);
    	}

    	int yes = 1;
    	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &yes, sizeof(int)))   // to overcome bind error
    	{
        	perror("setsockopt");
        	exit(1);
    	}
	
	
    	server.sin_family = AF_INET;
    	server.sin_addr.s_addr = INADDR_ANY;
    	server.sin_port = htons(PORT);

    	
    	if (bind(server_fd, (struct sockaddr *)&server, sizeof(server)) < 0)   // Bind the socket to port 
    	{
        	perror("bind failed");
        	exit(1);
    	}

    	
    	if (listen(server_fd, MAX_CLIENTS) < 0)    // Listen for incoming connections
    	{
        	perror("error while listening");
        	exit(1);
    	}

    	struct pollfd fds[MAX_CLIENTS + 1];  // array containing structures, MAX_CLIENTS + 1 is given for server + clients
    	
    	int des = 1;  // this indicates the number of file descriptors in use (client + server)
    	memset(fds, 0, sizeof(fds));  // we delete the free memory space in fds

    	// Add server socket to the pollfd structure
    	fds[0].fd = server_fd; // the server's file descriptor is in the 0th index
    	fds[0].events = POLLIN;  // POLLIN indicates ready for connection
    	printf("Waiting for connections...\n");
    	socklen_t addrlen = sizeof(server);
    	
    	while(1) 
    	{
        	
        	int cnt = poll(fds, des, -1);  // Call poll() to wait for events, here it waits for infinite time
        	
        	if (cnt < 0) 
        	{
            	perror("error in poll");
            	exit(1);
        	}

        
        for (int i = 0; i < des; i++)   // Check for events on each file descriptor
        {
        	if (fds[i].revents & POLLIN)   // if fds[i].revents has POLLIN set
                {
                	
                	if (fds[i].fd == server_fd)  // New incoming connection to the server
		        { 
		            if ((new_socket = accept(server_fd, (struct sockaddr *)&server, &addrlen))<0) 
		            {
		                perror("error while accepting...");
		                exit(1);
		            }

		            printf("New connection, socket fd is %d, ip is: %s,\n",new_socket, inet_ntoa(server.sin_addr));

		            // Adding new connection to the pollfd structure
		            fds[des].fd = new_socket;   // add new socket descriptor in the array
		            fds[des].events = POLLIN;  
		            des++;    // increase the number of file descriptors in use as we have established a new client connection
                } 
                
                else   // read and send data 
                { 
                    int client_socket = fds[i].fd;
                    valread = read(client_socket, buffer, 1024);
                    // Client disconnected
                    if (valread == 0)
                    { 
                        printf("Client disconnected\n");
                        close(client_socket);
                        
                        fds[i].fd = -1;   // we remove the file descriptor
                    } 
                    else
                    {
                        printf("Message recieved from client: %s \n",buffer);
                        printf("Sending Message back to the client\n");
                        send(fds[i].fd, buffer, valread, 0);  // sends the message back to the client
                        memset(&buffer, '\0', sizeof(buffer));  // reset the buffer with '\0'
                    }
                }
            }
        }
    }
    return 0;
}
