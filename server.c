#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <stdio.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h>
#include <time.h> 
#define MAX 80 
#define PORT 8080 
#define SA struct sockaddr 

// #define HEADER_SIZE 3
  
// Function designed for chat between client and server. 
void func(int sockfd) 
{ 
        // u_char header_info[HEADER_SIZE];

        // read(sockfd, header_info, sizeof(header_info));

        // u_char n = header_info[0];
        // u_char width = header_info[1];
        // u_char height = header_info[2];

        int n = 0;
        read(sockfd, &n, sizeof(n));
        int width = 0;
        read(sockfd, &width, sizeof(width));
        int height = 0;
        read(sockfd, &height, sizeof(height));
        n = ntohl(n);
        width = ntohl(width);
        height = ntohl(height);
        

        printf("Received header:\n Number of matrices = %d\n Width of each = %d\n Height of each = %d\n", n, width, height);

        int buff_size = width * height;
        u_char buff[buff_size]; 
        u_char out[buff_size];

        clock_t begin = clock();
        
        for(size_t i = 0; i < n; i++) {
            read(sockfd, buff, sizeof(buff));

            for(size_t j = 0; j < height; j++) {
                for(size_t k = 0; k < width; k++) {
                    int index = j * width + k + j;
                    if (index < (j + 1) * width) {
                        out[j*width +k] = buff[index];
                        printf("\t%x", buff[index]);
                    } else {
                        out[j*width +k] = buff[index - width];
                        printf("\t%x", buff[index - width]);
                    }
                    // printf("\t%x", buff[ j * width + k]);
                }
                printf("\n");
            }
            write(sockfd, out, sizeof(out));
            printf("\n");
        }

        clock_t end = clock();
        long int time_spent = (long int)(end - begin);
        printf("Elapsed: %ld ms\n", time_spent);
} 
  
// Driver function 
int main() 
{ 
    int sockfd, connfd, len; 
    struct sockaddr_in servaddr, cli; 
  
    // socket create and verification 
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        printf("socket creation failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully created..\n"); 
    bzero(&servaddr, sizeof(servaddr)); 
  
    // assign IP, PORT 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(PORT); 
  
    // Binding newly created socket to given IP and verification 
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
        printf("socket bind failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully binded..\n"); 
  
    // Now server is ready to listen and verification 
    if ((listen(sockfd, 5)) != 0) { 
        printf("Listen failed...\n"); 
        exit(0); 
    } 
    else
        printf("Server listening..\n"); 
    len = sizeof(cli); 
  
    // Accept the data packet from client and verification 
    connfd = accept(sockfd, (SA*)&cli, &len); 
    if (connfd < 0) { 
        printf("server acccept failed...\n"); 
        exit(0); 
    } 
    else
        printf("server acccept the client...\n"); 
  
    // Function for chatting between client and server 
    func(connfd); 
  
    // After chatting close the socket 
    close(sockfd); 
} 