
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
#define MAX_SIZE 10240

void handle_postrequest(int, char[]);
void handle_client(int);


void handle_postrequest(int sockfd, char files[])   
{
    FILE *file = fopen("post_data.txt", "a");  // we open this file to store the body of the post request
    if (file == NULL) 
    {
        perror("Failed to open file");
        exit(1);
    }
    FILE *file2 = fopen(files, "r");
    if(file2 == NULL)
    {
    	perror("PAGE NOT FOUND");
    	exit(1);
    }
    int ch;
    printf("Contents of the file '%s':\n", files);
    while ((ch = fgetc(file2)) != EOF) 
    {
    	
        printf("%c", ch);
        fprintf(file, "%c\n", ch);  // we print the body of the request in the above file
    }
    
    fclose(file);
    fclose(file2);

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
{	char buffer[MAX_SIZE];   // buffer to store 
	int n;

	if ((n = recv(sockfd, buffer, MAX_SIZE - 1, 0)) < 0)   // receives the input request
	{
		perror("Error in receiving request");
		exit(1);
	}
	buffer[n] = '\0'; 	
	char *body = strstr(buffer, "filename"); // now character pointer body points to the starting location of the string "name = filename"
	printf("%s\n", buffer);
	/*char pattern[1000];
	int start=0,ind=0;
	while(*body!='\r')
	{
		if(start==1) 
		{
			pattern[ind++]=*body;
		}
		if(*body=='=') start=1;
		body++;
		
	}
	printf("%s\n", pattern); */
	body += 13;  // to skip the filename and \r\n\r\n, this takes to skip 13 characters
	char *body2 = strstr(body, "-");
	int len = body2 - body - 2; // to skip the - and \n
	char buffer2[len];
	int i = 0;
	while(len)
	{
		buffer2[i] = *body;
		body++;
		len--;
		i++;
	}
	buffer[len] = '\0';
	printf("%s\n", buffer2);
	//handle_postrequest(sockfd, buffer2);  // after we get the body of the request we call the handle_post_request 
	memset(&buffer, '\0', strlen(buffer));	
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
    //printf("Send POST request using curl command...\n");
    

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
