#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<strings.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define LOCALHOST "10.39.27.100" 
#define PORT 8060
char FILE_DIR[1000];
#define MAX_BUFFER_SIZE 50000

void send_http_response(int socket_client, const char* response)
{
	printf("Sending response to the proxy\n");
    	send(socket_client, response, strlen(response), 0);
}

void handle_http_request(int socket_client, const char* request)
{
	printf("In handle_http_request\n");
	//printf("%s\n", request);

    	char resource[MAX_BUFFER_SIZE];
    	char *base1 = strstr(request, "//");
    	base1 += 2;
    	base1 = strstr(base1, "/");
    	base1 += 1;
    	char *base2 = strstr(base1, " ");
    	
    	char file_name[100];
    	memset(&file_name, '\0', strlen(file_name));
    	
    	int i = 0;
    	while(base1 != base2)
  		file_name[i++] = *base1++;
  	file_name[i] = '\0';
  	printf("File Name: %s\n", file_name);
  	strcat(FILE_DIR, file_name);
  	
  	printf("Address : %s\n", FILE_DIR);
  	FILE *file = fopen(FILE_DIR, "r");
  	
  	memset(&FILE_DIR, '\0', sizeof(FILE_DIR));
    	if (file != NULL)
	{
	printf("file opening successful\n");
        const char* response_header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length:1024\r\n\r\n";
        send_http_response(socket_client, response_header);
        
        char file_buffer[2*MAX_BUFFER_SIZE];
        size_t bytesRead;
        while ((bytesRead = fread(file_buffer, 1, sizeof(file_buffer), file)) > 0)
            send(socket_client, file_buffer, bytesRead, 0);
        fclose(file);
    	}
	else
	{
		printf("Unable to open file\n");
        	const char* response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n404 - Not Found\n";
        	send_http_response(socket_client, response);
    	}
}

int main ()
{
	int socket_server,socket_client;
	char request[MAX_BUFFER_SIZE];
	
	if((socket_server = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf ("Failed to create Socket\n");
		exit(1);
	}
	
	struct sockaddr_in server_address;
	
	int yes = 1;
    	if (setsockopt(socket_server, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &yes, sizeof(yes)))   // to overcome bind error
    	{
        	perror("setsockopt");
        	exit(1);
   	}
	
	bzero(&server_address, sizeof(server_address));
	
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(PORT);
	server_address.sin_addr.s_addr = INADDR_ANY;
	
	if((bind(socket_server,(struct sockaddr*) &server_address, sizeof(server_address))) < 0)
	{
		printf("Failed to Bind...\n");
		exit(1);
	}
	
	if(listen(socket_server, 5) < 0)
	{
		printf("Failed to listen...\n");
		exit(1);
	}
	
	printf("Server listening at port: %d...\n", PORT);
	
	while(1)
	{
		if((socket_client = accept(socket_server, NULL, NULL)) < 0)
		{
			printf("Connection to Client failed...\n");
			exit(1);
		}

		printf("Connection to proxy successful\n");
		
		if(read(socket_client, request, sizeof(request)) < 0)
		{
			printf("HTTP request read operation failed...\n");
			exit(1);
		}
		
		printf("\nRequest from the HTTP Proxy: %s\n",request);
		
		handle_http_request(socket_client, request);
		
		close(socket_client);
	}
	
	return 0;
}
