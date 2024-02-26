// using select

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h> 

#define MAX_CLIENTS 10  // Maximum number of clients which can be connected to the server concurrently
#define BUFFER_SIZE 1024

int main() 
{
    int server_socket, client_socket[MAX_CLIENTS], max_clients = MAX_CLIENTS, activity, i, valread, sd;
    int max_sd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    int addrlen; // Declare addrlen variable

    fd_set readfds;  // Set of socket descriptors

    
    for (i = 0; i < max_clients; i++)   // Initialize client sockets
        client_socket[i] = 0;
    

   
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)    // Create a server socket
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options to allow multiple connections
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&i, sizeof(i)) < 0) {
        perror("Setsockopt failed");
        exit(EXIT_FAILURE);
    }

    // Initialize server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(9999);

    // Bind the socket to localhost
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server started. Waiting for connections...\n");

    while (1) {
        // Clear the socket set
        FD_ZERO(&readfds);

        // Add the server socket to the set
        FD_SET(server_socket, &readfds);
        max_sd = server_socket;

        // Add child sockets to the set
        for (i = 0; i < max_clients; i++) {
            // Socket descriptor
            sd = client_socket[i];

            // If valid socket descriptor then add to read list
            if (sd > 0) {
                FD_SET(sd, &readfds);
            }

            // Highest file descriptor number, needed for select function
            if (sd > max_sd) {
                max_sd = sd;
            }
        }

        // Wait for activity on any of the sockets
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR)) {
            printf("Select error\n");
        }

        // If something happened on the server socket, then it's an incoming connection
        if (FD_ISSET(server_socket, &readfds)) {
            int new_socket;
            struct sockaddr_in client_addr;
            addrlen = sizeof(client_addr);

            // Accept the incoming connection
            if ((new_socket = accept(server_socket, (struct sockaddr *)&client_addr, (socklen_t *)&addrlen)) < 0) {
                perror("Accept failed");
                exit(EXIT_FAILURE);
            }

            // Inform the user of the incoming connection
            printf("New connection, socket fd is %d, IP is: %s, port is: %d\n", new_socket, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

            // Add new socket to array of sockets
            for (i = 0; i < max_clients; i++) {
                if (client_socket[i] == 0) {
                    client_socket[i] = new_socket;
                    printf("Adding to list of sockets as %d\n", i);
                    break;
                }
            }
        }

        // Check for IO operation on other sockets
        for (i = 0; i < max_clients; i++) {
            sd = client_socket[i];
            if (FD_ISSET(sd, &readfds)) {
                if ((valread = read(sd, buffer, BUFFER_SIZE)) == 0) {
                    // Somebody disconnected, get his details and print
                    getpeername(sd, (struct sockaddr *)&server_addr, (socklen_t *)&addrlen);
                    printf("Host disconnected, IP %s, port %d\n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));

                    // Close the socket and mark as 0 in list for reuse
                    close(sd);
                    client_socket[i] = 0;
                }
                else {
                    // Echo back the message that came in
                    buffer[valread] = '\0';
                    send(sd, buffer, strlen(buffer), 0);
                }
            }
        }
    }

    return 0;
}

