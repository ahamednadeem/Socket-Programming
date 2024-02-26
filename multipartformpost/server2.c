
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


void handle_postrequest(int, char[], char[], char[]);
void handle_client(int);


void handle_postrequest(int sockfd, char file_name[], char content[], char message_sent[])   
{
    FILE *file = fopen("post_data.txt", "a");  // we open this file to store the body of the post request
    if (file == NULL) 
    {
        perror("Failed to open file");
        exit(1);
    }
    //printf("%s\n", content);
    int j;
    for(j = 0; j < strlen(message_sent); j++)
    	fprintf(file, "%c", message_sent[j]);
    	
    FILE *file2 = fopen(file_name, "a");
    if(file2 == NULL)
    {
    	perror("Failed to open file");
    	exit(1);
    }
    
    int i;
    //printf("%s\n", content);
    for(i = 0; i < strlen(content); i++)    // copy the content in the file
    	fprintf(file2, "%c", content[i]);
    
    fclose(file);
    fclose(file2);



    // send the response to local host	
    const char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n\
    <html>\
    	<body>\
    		<h1>POST Request Received and Stored</h1>\
    		<h2>Thank You</h2>\
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
	//printf("%s\n", buffer);
	
	if(strncmp(buffer, "GET /favicon.ico", 16) == 0)  // to handle GET Method
		return ; 
	
	// First we extract the filename
	char *filename = strstr(buffer, "filename=");	
	filename += 10;
	
	char *filename_end = strstr(filename, "\n");
	
	
	int len1 = filename_end - filename - 2;
	char buffer1[100];
	memset(&buffer1, '\0', strlen(buffer1));
	
	
	
	int k = 0;
	while(len1--)
	{
		buffer1[k++] = *filename++;
	}
	buffer1[len1] = '\0';
	
	printf("FIELD 1: FILE \nFile Name: %s \n", buffer1); 
	
	
	// For extracting the contents in the file
	char *body = strstr(buffer, "Content-Type"); 
	body += 1;
	body = strstr(body, "Content-Type");
	body += 1;
	body = strstr(body, "\n");
	body += 1;
	
	char *body2 = strstr(body, "---");  // marks the end of the code
	int len = body2 - body;
	
	char buffer2[4096];
	memset(&buffer2, '\0', strlen(buffer2));
	int i = 0;
	while(len--)
		buffer2[i++] = *body++;
	buffer2[len] = '\0';
	
	printf("Contents in the File %s is %s \n", buffer1, buffer2);
	
	
	// For extracting the message we have sent
	char *msg = strstr(buffer, "message");
	msg += 10;
	char *msg2 = strstr(msg, "---");
	int len2 = msg2 - msg;
	
	int j = 0;
	char buffer3[1000];
	memset(&buffer3, '\0', strlen(buffer3));
	
	while(len2--)
		buffer3[j++] = *msg++;
	buffer3[len2] = '\0';
	
	printf("FIELD 2: MESSAGE \nMessage Received is %s \n", buffer3); 
	
	handle_postrequest(sockfd, buffer1, buffer2, buffer3);
	
	
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

    memset(&serv_addr, '\0', sizeof(serv_addr));   
    
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
