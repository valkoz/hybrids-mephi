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

int create(char* addr, char* port) {
    int sockfd; 
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
    servaddr.sin_addr.s_addr = inet_addr(addr); 
    servaddr.sin_port = htons(port); 
  
    // connect the client socket to server socket 
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) { 
        printf("connection with the server failed...\n"); 
        exit(0); 
    } 
    else
        printf("connected to the server..\n");
    
    return sockfd;
}

void send_data(int sockfd, int count, int width, int height) 
{ 
    int n = htonl(count);
    write(sockfd, &n, sizeof(n));
    int w = htonl(width);
    write(sockfd, &w, sizeof(w));
    int h = htonl(height);
    write(sockfd, &h, sizeof(h));


    // u_char header[3] = {MATRIX_COUNT, MATRIX_SIZE_WIDTH, MATRIX_SIZE_HEIGHT};

    // write(sockfd, header, sizeof(header)); 

    int size = count * width * height;
    int sz = width * height;
    u_char buff[size]; 
    bzero(buff, sizeof(buff)); 
    for(size_t i = 0; i < count; i++)
    {
        for(size_t j = 0; j < sz; j++)
        {
            if (j % width == 0) {
                printf("\n");
            }
            if (j % sz == 0) {
                printf("\n");
            }

            buff[i*sz + j] = randomByte();     
            printf("\t%x", buff[i*sz + j]);       
        }
    }

    write(sockfd, buff, sizeof(buff));
}

void receive_data(int sockfd, int count, int width, int height) {

    int size = width * height;

    u_char buff[size];
    bzero(buff, sizeof(buff));
 
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
    int sockfd;
    char* host = argv[1];
    int port = atoi(argv[2]);
    int count = atoi(argv[3]);
    int height = atoi(argv[4]);
    int width = atoi(argv[4]);
    sockfd = create(host, port);
    send_data(sockfd, count, width, height); 
    receive_data(sockfd, count, width, height);
    close(sockfd); 
} 
