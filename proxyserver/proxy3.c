// HTTP Proxy

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<error.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<unistd.h>

#define MAX 65536
#define PORT 8090

void proxy_to_server(int socket_client)   // handle the incoming request, retrieve the hostname and port number
{
	int flag = 0;
	char request[MAX];
	int socket_server;
	size_t r;
  	struct addrinfo *hserv, hints, *p;
  	int n;
  	char host_name[100];
  	char port_number[6] = "80";
  	

	if((r = read(socket_client, request, MAX)) > 0)
	{
		printf("Received Request from client is: %s\n", request);
	
		char *get = strstr(request, "Host:");
		get += 6;
		
		char *get2 = strstr(get, "\n");
		
		char* a;
		a = get;
		
		while(a != get2) 
		{
			
			if(*a == ':')
			{
				flag = 1;
				printf("\nflag is set\n");
				break;
			}
			a++;
		}
			
		if(flag)   // if the request is from a local host
		{
			char *base1 = strstr(request, "//");
			base1 += 2;
			char *base2 = strstr(base1, ":");
		
			int i = 0;
			while(base1 != base2)
				host_name[i++] = *base1++;
			host_name[i] = '\0';

			base2 += 1;
			
			base1 = strstr(base1, "/");
		
			int j = 0;
			while(base2 != base1)
			port_number[j++] = *base2++;
			port_number[j] = '\0';
			
			printf("\nServer hostname: %s\n",host_name);
			printf("\nServer port: %s\n", port_number);	
		}
		else  // if the request is from remote server
		{
			int i = 0;
			while(get != get2)
				host_name[i++] = *get++;
			i -= 1;
			host_name[i] = '\0';

			printf("\nHost name: %s\n", host_name);
			printf("\nPort Number: %s\n", port_number);
		} 	
		
		memset(&hints, '\0' ,sizeof(hints));  
		printf("wfbre");
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
				
		if((n = getaddrinfo(host_name, port_number, &hints, &hserv)) != 0)  // gets address information of the server
		{
			herror("Getaddrinfo error...\n");
			exit(1);
		}
		
		for(p = hserv; p != NULL; p = p-> ai_next)  // to get the perfect address to connect
		{
			socket_server = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
			if(socket_server < 0)
			{
				perror("Socket creation error...\n");
				printf("\n\n\n\n\n\n");
				continue;
			}
			if(connect(socket_server, p->ai_addr, p->ai_addrlen) < 0)
			{
				close(socket_server);
				perror("Connection error...\n");
				printf("\n\n\n\n\n\n");
				continue;			
			}
			break;	// if connected correctly then it breaks out
		}
		
		if(p == NULL)
		{
			printf("-------Not connected-------\n");
			exit(1);			
		}
		
		printf("Connected to the server...\n");
		
		if(send(socket_server, request, MAX, 0) < 0)  // sends the request to the server
		{
			perror("Send error...\n");
			exit(1);
		}
		printf("\nRequest sent to the server..\n");
		
		int byte_count;
		
		char response[MAX];
		memset(&response,'\0',sizeof(response));
		
		while((byte_count = recv(socket_server, response, MAX, 0)) > 0)  // receive the response from the server
		{
			printf("\nResponse received from the server: \n %s", response);
			printf("Sending response back to the browser\n");
			send(socket_client, response, byte_count, 0);  // send back the response to the client/browser
		}	
		printf("Response sent to Browser.... \n");
		close(socket_server);	
		
	}
	close(socket_client); 
	
}


int main()
{
	int socket_server, socket_client;
	socklen_t client_len;
	
	struct sockaddr_in server_address, client_address;
	socket_server = socket(AF_INET, SOCK_STREAM, 0);
	
	if(socket_server == -1)
	{
		perror("\nError in creating socket...\n");
		exit(1);
	}
	
	memset(&server_address, 0, sizeof(server_address));
	
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(PORT);
	server_address.sin_addr.s_addr = INADDR_ANY;
	
	printf("\nSocket created...\n");
	
	int yes = 1;
    	if (setsockopt(socket_server, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &yes, sizeof(yes)))   // to overcome bind error
    	{
        	perror("setsockopt");
        	exit(1);
   	}
	
	if(bind(socket_server,(struct sockaddr*)&server_address,sizeof(server_address)) < 0)  
	{	
		perror("\nBind error...\n");
		exit(1);
	}
	
	if(listen(socket_server,5) == -1)
	{
		perror("\nlisten error...\n");
		exit(1);
	}
	
	printf("Proxy server listening at port %d...\n",PORT);
	
	client_len = sizeof(client_address);
	
	while(1)
	{
		socket_client = accept(socket_server, (struct sockaddr*)&client_address, &client_len);  // accept incoming connections
		if(socket_client == -1)
		{
			perror("\nClient connection error...\n");
			exit(1);		
		}
		
		printf("\nClient connection successful...\n");
		
		proxy_to_server(socket_client);
	}
	close(socket_server);  // close the socket
	return 0;
}
