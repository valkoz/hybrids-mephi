// ./client 127.0.0.1 8080 1024 1024 omp.txt s

#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <stdbool.h>
#include <time.h> 
#include <errno.h>
#include <string.h>
#define SA struct sockaddr 

unsigned char randomByte()
{
  return (unsigned char) randInRange( 0, 255 );
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

void send_data(int sockfd, int count, int width, int height, int silent) 
{ 
    printf("Sending data...");

    int n = htonl(count);
    write(sockfd, &n, sizeof(n));
    int w = htonl(width);
    write(sockfd, &w, sizeof(w));
    int h = htonl(height);
    write(sockfd, &h, sizeof(h));

    int sz = width * height;
    unsigned char buff[sz]; 
    for(size_t i = 0; i < count; i++)
    {
        bzero(buff, sizeof(buff)); 
        for(size_t j = 0; j < sz; j++)
        {
	    buff[j] = randomByte();    
	    
	    if (!silent) {
	    	if (j % width == 0) {
                    printf("\n");
                }
                if (j % sz == 0) {
                    printf("\n");
                }
		printf("\t%x", buff[j]);
	    }
        }
        write(sockfd, buff, sizeof(buff)); 
    }
    printf("\nData have been send.");
}

double receive_data(int sockfd, int count, int width, int height, int silent) {

    int size = width * height;

    unsigned char buff[size];
    bzero(buff, sizeof(buff));
 
    printf("\nReceiving result...");

    unsigned char *in = malloc(sizeof(unsigned char) * count * width * height);

    for(size_t i = 0; i < count; i++)
    {
        read(sockfd, in + i * size, sizeof(buff)); 
        if (!silent) {
            for(size_t j = 0; j < size; j++) {
	    	if (j % width == 0) {
                    printf("\n");
                }
                if (j % size == 0) {
                    printf("\n");
                }
		printf("\t%x", in[i * size + j]);
            }
	}
    }

    double time = 0;
    read(sockfd, &time, sizeof(time));

    printf("\nResult received.");
    free(in);
    
    return time;
} 

void save_report(char* file_name, long int size_bytes, double time_ms) {
    FILE *file;
    file = fopen(file_name, "ab");
    if (!file) {
        printf("something went wrong: %s", strerror(errno));
	exit(1);
    }
    fprintf(file, "%ld:%lf\n", (long int) size_bytes, (double) time_ms);
    fclose(file);
    printf("Report saved.\n"); 
}
  
int main(int argc, char **argv) 
{ 
    int sockfd;
    char* host = argv[1];
    int port = atoi(argv[2]);
    int count = atoi(argv[3]);
    int height = atoi(argv[4]);
    int width = atoi(argv[4]);
    char* file_name = argv[5];
    bool silent = false;
    if (argv[6])
	silent = true;

    sockfd = create(host, port);

    clock_t begin = clock();
    send_data(sockfd, count, width, height, silent); 
    double time = receive_data(sockfd, count, width, height, silent);
    clock_t end = clock();
    printf("\nElapsed: %ld ms", (long int)(end - begin));

    close(sockfd);
    printf("\nSocket successfully closed.\n"); 
    save_report(file_name, count * height * width, time);
} 
