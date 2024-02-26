#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>

#define PORT 8080
#define WEBROOT "./Contents" // This is the Directory containing HTML files

int main() 
{
    char buffer[1024];
    memset(&buffer,'\0',1024);  // clears the buffer 
    
    //creating server socket
    int server_fd;
    server_fd = socket(AF_INET,SOCK_STREAM,0);
    
    if(server_fd < 0)
    {
        perror("socket failed");
        exit(1);
    }


    int yes = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,&yes, sizeof(int)) < 0)   // Used to overcome bind error
    {
        perror("setsockopt failed");
        exit(1);
    }

    struct sockaddr_in server;  
    
    server.sin_family = AF_INET;  // for ipv4
    server.sin_addr.s_addr = INADDR_ANY;  // to accept connections from any clients
    server.sin_port = htons(PORT);   // convert port number to network byte order 


   
    if (bind(server_fd, (struct sockaddr *)&server, sizeof(server))<0)   // Bind socket to server
    {
        perror("bind failed");
        exit(1);
    }


    if (listen(server_fd, 1) < 0)   // make it to listen which makes it ready to accpet connections
    {
        perror("listen failed");
        exit(1);
    }
    
    while(1)
    {
        struct sockaddr_in client;
        socklen_t len = sizeof(client);

        int client_fd = accept(server_fd, (struct sockaddr *)&client, &len);   // accepting incoming connections.
        if(client_fd < 0)
        {
            perror("accept failed");
            exit(1);
        }

        recv(client_fd, buffer, 1024, 0);    //recieving http request from the client (web browser), the HTTP request is stored in the buffer

        char file_name[100];
        sscanf(buffer, "GET /%s", file_name);  // we are extracting the filename from the http request 
        				
   	
        char file_path[1000];
        sprintf(file_path,"%s/%s",WEBROOT,file_name); // file_path contain the whole path to assess the file

        int fd = open(file_path, O_RDONLY,0);   // we open the file with the given filepath
        char buff[1024];
        int r; 
        
        if(fd >= 0)      // If the given file is present, this case passes
        {
            char header[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length:1024\r\n\r\n";
            send(client_fd , header , strlen(header) , 0);  // We send the client the header, hence the output is printed in HTML format
            while((r = read(fd, buff, 1024)) > 0)
            {
                send(client_fd, buff, r, 0);
            }
            close(fd);
        }
        else  // The given file is not found, hence default error page is displayed
        {
            char header[] = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nContent-Length:1024\r\n\r\n";
            send(client_fd, header, strlen(header), 0); 
            strcpy(file_name,"error.html");   //Error 404 message is present in error.html 
            sprintf(file_path,"%s/%s",WEBROOT,file_name);  //file_path now contains the whole path to the error message html file
            
            int fd = open(file_path, O_RDONLY,0);
            while((r = read(fd, buff, 1024)) > 0)
            {
                send(client_fd, buff, r,0);
            }
       
        }
        close(client_fd);
    }
    close(server_fd);

    return 0;
}
