#include <netdb.h>
#include <unistd.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>


#define MAX 65536         
#define PORT 8090        // the port number of the proxy server
#define MAX_CLIENTS 100  // maximum number of clients that can be connected

void handle_connect_request(int socket_client, int socket_server) 
{
	const char *success_response = "HTTP/1.1 200 OK\r\n\r\n";
    	send(socket_client, success_response, strlen(success_response), 0);  // we send the success message to the browser/client t

    	printf("Connected to server\n");

    	while (1) 
    	{
        	struct pollfd fds[2];
        	fds[0].fd = socket_client;     // contains the client socket descriptor
        	fds[0].events = POLLIN;

        	fds[1].fd = socket_server;     // contains the server socket descriptor
        	fds[1].events = POLLIN;

        	int ret = poll(fds, 2, -1); // 2 is the length of fds and -1 means the poll waits for infinite time for the client

        	if (ret < 0) 
        	{
            		perror("Poll error \n");
            		exit(1);
        	}

       	for (int fd = 0; fd < 2; fd++) 
       	{
            		if (fds[fd].revents & POLLIN) 
            		{
                		char buffer[MAX];
                		ssize_t bytes_received = recv((fd == 0) ? socket_client : socket_server, buffer, sizeof(buffer), 0);
                		// if fd == 0, the request from the client is stored in the buffer and sent to the server
                		// if fd == 0, the reaponse from the server is stored in the buffer and sent to the client
            
                		if (bytes_received <= 0) 
                		{
                    			break;
                		}

                	send((fd == 0) ? socket_server : socket_client, buffer, bytes_received, 0);
            		}
        	}
	}

    	close(socket_client);
    	close(socket_server);
}

void proxy_to_server(int socket_client)   // checks if it is a connect request or a non connect request, then extracts the host name and port number. Gets the destination server address and connects to the server
{
    char request[MAX]; // to store the request
    ssize_t r;
    if ((r = read(socket_client, request, MAX)) > 0)   // read the incoming request from the client/browser
    {
    	printf("The request Received is: \n %s\n", request);

        if (strncmp(request, "CONNECT", 7) == 0)   // checking if it is a connect request
        {
        	printf("Received a CONNECT request\n");
        	char *host = strstr(request, "CONNECT");
        	host += 8;
        
        	char host_name[1000];
        	memset(&host_name, '\0', strlen(host));
  
        	int i = 0;
        	while(*host != ':')  // extracting the host name from the connect request
        		host_name[i++] = *host++;
        	host_name[i] = '\0';
        	host += 1;
        	
        	printf("Host Name: %s \n", host_name);
        
        	char port_str[6];
        	memset(&port_str, '\0', strlen(port_str));
        	
        	i = 0;
        	while(*host != ' ')   // extracting the port number from the connect request
        		port_str[i++] = *host++;
        	port_str[i] = '\0';
        	printf("Port number: %s \n", port_str);
        	
            	int port = atoi(port_str);

            	printf("Proxy Connecting to %s:%d...\n", host_name, port);

            	int socket_server = socket(AF_INET, SOCK_STREAM, 0);  // creating a new socket for the proxy to connect to the server

            	if (socket_server < 0) 
            	{
                	perror("Failed to create server socket");
                	close(socket_client);
                	return;
            	}

            	struct sockaddr_in server_addr;
            	
            	memset(&server_addr, 0, sizeof(server_addr));
            	
            	server_addr.sin_family = AF_INET;
            	server_addr.sin_port = htons(port);

            	struct addrinfo hints, *server_info;

            	memset(&hints, 0, sizeof(hints));
            	
            	hints.ai_family = AF_INET;
            	hints.ai_socktype = SOCK_STREAM;

            	int getaddrinfo_result = getaddrinfo(host_name, port_str, &hints, &server_info); // get the address information about the remote server 
		// the information is stored in server_info
		
            	if (getaddrinfo_result != 0) 
            	{
                	//fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(getaddrinfo_result));
                	printf("Get addrinfo error\n");
                	close(socket_server);
                	close(socket_client);
                	return;
            	}

            	struct sockaddr_in *server_addr_in = (struct sockaddr_in *)server_info->ai_addr; //casts the ai_addr member of a structure referenced by server_info to a pointer of type struct sockaddr_in
            	
            	memcpy(&server_addr.sin_addr.s_addr, &server_addr_in->sin_addr, sizeof(struct in_addr));  // copy memory from one location to another
		// &server_addr.sin_addr.s_addr --> this is where the ip address of the server is to be stored
		// &server_addr_in->sin_addr ---> holds the ip address of the server
		// memcpy call is copying the IP address from server_addr_in->sin_addr to server_addr.sin_addr.s_addr.
            	freeaddrinfo(server_info);  // deallocating the memory associated with the server_info pointer, which was previously allocated by getaddrinfo().

            	if (connect(socket_server, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)   // connect to the server
            	{
                	perror("Failed to connect to the server");
                	close(socket_server);
                	close(socket_client);
                	return;
            	}

            	handle_connect_request(socket_client, socket_server); // we send the client and servers socket descriptors
            	
        } 
        
        else 
        {
            	printf("Non Connect Request Received\n");
            	close(socket_client);
        }
    }
}

int main() 
{
    	int socket_server, socket_client;
    	socklen_t client_len;
    	struct sockaddr_in server_address, client_address;

    	socket_server = socket(AF_INET, SOCK_STREAM, 0);  // create socket for server

    	if (socket_server == -1) 
    	{
        	perror("Error in creating socket\n");
        	exit(1);
    	}
	printf("Socket created\n");
    
    	int yes = 1;
    	if (setsockopt(socket_server, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)   // to overcome bind error
    	{
        	perror("setsockopt error\n");
        	exit(1);
    	}

    	memset(&server_address, '\0', sizeof(server_address));
    	
    	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(PORT);
	server_address.sin_addr.s_addr = INADDR_ANY;

    	if (bind(socket_server, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)   // bind the server to the address
    	{
        	perror("Failed to bind\n");
        	exit(1);
    	}

    	printf("Binding done\n");

    	if (listen(socket_server, 5) == -1)   // making server ready to listen to incoming connections
    	{
        	perror("Failed to listen\n");
        	exit(EXIT_FAILURE);
    	}

    	printf("Proxy server listening at port %d...\n", PORT);
    	client_len = sizeof(client_address);

    	for (int i = 0; i < MAX_CLIENTS; i++) 
    	{
        	socket_client = accept(socket_server, (struct sockaddr *)&client_address, &client_len);  // accepting client connections
        	if (socket_client == -1) 
        	{
            		perror("Client connection error\n");
            		continue;
       	}

        printf("The client is connected\n");

        pid_t pid = fork();  // creates a new process by duplicating the existing process, pid contains the process ID of the child

        if (pid == -1) 
        {
            perror("Fork Error\n");
            close(socket_client);
            continue;
            
        } 
        else if (pid == 0)  // Child process, socket_server is not required in child process hence close the socket_server
        {
            close(socket_server);   
            proxy_to_server(socket_client);
            exit(1);
        } 
        else        // Parent process
            close(socket_client);   // parent process handles server socket , only child handles client socket so we close the socket_client
        
    }

    close(socket_server);

    return 0;
}
