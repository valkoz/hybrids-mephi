// ./client 127.0.0.1 8080 64 1024

#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#define SA struct sockaddr 

// #define MATRIX_COUNT 8192
// #define MATRIX_SIZE_WIDTH 1024
// #define MATRIX_SIZE_HEIGHT 1024

u_char randomByte()
{
  return (u_char) randInRange( 0, 255 );
}

int randInRange( int min, int max )
{
  double scale = 1.0 / (RAND_MAX + 1);
  double range = max - min + 1;
  return min + (int) ( rand() * scale * range );
}

void func(int sockfd, int count, int width, int height) 
{ 
    int n = htonl(count);
    write(sockfd, &n, sizeof(n));
    int w = htonl(width);
    write(sockfd, &w, sizeof(w));
    int h = htonl(height);
    write(sockfd, &h, sizeof(h));


    // u_char header[3] = {MATRIX_COUNT, MATRIX_SIZE_WIDTH, MATRIX_SIZE_HEIGHT};

    // write(sockfd, header, sizeof(header)); 

    int size = width * height;
    u_char buff[size]; 
    for(size_t i = 0; i < count; i++)
    {
        bzero(buff, sizeof(buff)); 
        for(size_t j = 0; j < size; j++)
        {
            if (j % width == 0) {
                printf("\n");
            }
            if (j % size == 0) {
                printf("\n");
            }

            buff[j] = randomByte();     
            printf("\t%x", buff[j]);       
        }

        write(sockfd, buff, sizeof(buff)); 
        
    }

    printf("\nResult:\n");

    for(size_t i = 0; i < count; i++)
    {
        printf("\n");
        read(sockfd, buff, sizeof(buff)); 
        for(size_t j = 0; j < height; j++) {
            for(size_t k = 0; k < width; k++) {
                printf("\t%x", buff[j*width +k]);
            }
            printf("\n");
        }
    }
    
} 
  
int main(int argc, char **argv) 
{ 
    int sockfd, connfd; 
    struct sockaddr_in servaddr, cli; 
  
    // socket create and varification 
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
    servaddr.sin_addr.s_addr = inet_addr(argv[1]); 
    servaddr.sin_port = htons(atoi(argv[2])); 
  
    // connect the client socket to server socket 
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) { 
        printf("connection with the server failed...\n"); 
        exit(0); 
    } 
    else
        printf("connected to the server..\n"); 
  
    // function for chat 
    func(sockfd, atoi(argv[3]), atoi(argv[4]), atoi(argv[4])); 
  
    // close the socket 
    close(sockfd); 
} 
