#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <stdio.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h>
#include <unistd.h>
#include <time.h> 
#define PORT 8080 
#define SA struct sockaddr 

// nvcc cuda.cu -o Cuda, ./Cuda

int create() {
    int sockfd, connfd; 
    socklen_t len;
    struct sockaddr_in servaddr, cli; 
  
    // socket create and verification 
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        printf("socket creation failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully created..\n"); 
    
    //if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
	printf("setsockport(SO_REUSEADDR) failed \n");

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
    
    return connfd;
}

__global__ void process(long grid_size, long treads, int n, int height, int width, unsigned char *in, unsigned char *out) {
    int i, j, k;
    for(i = 0; i < n; i++) {     
        for(j = 0; j < height; j++) {
            for(k = 0; k < width; k++) {
                int current = i * width * height + j * width + k;
                if (k + j < width) {
                    out[current] = in[current + j];
                } else {
                    out[current] = in[current + j - width];
                }
            }
        }
    }
}
  
// Driver function 
int main(int argc, char **argv) 
{ 
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    int sockfd;
    int n, width, height;
    
    long grid_size = 2;
    long treads_size = 2;    

    unsigned char *in = NULL;
    unsigned char *cuda_in = NULL;
    unsigned char *cuda_out = NULL;
    unsigned char *out = NULL;
    //struct timeval start, end;

    sockfd = create();
    
    read(sockfd, &n, sizeof(n));
    read(sockfd, &width, sizeof(width));
    read(sockfd, &height, sizeof(height));
    n = ntohl(n);
    width = ntohl(width);
    height = ntohl(height);
        
    printf("Received header:\n Number of matrices = %d\n Width of each = %d\n Height of each = %d\n", n, width, height);

    in = (unsigned char*) malloc(sizeof(unsigned char) * n * width * height);
    out = (unsigned char*) malloc(sizeof(unsigned char) * n * width * height);

    for (size_t i = 0; i < n; i++) {
        read(sockfd, in + i * height * width, sizeof(unsigned char) * height * width);
    }
    
    //gettimeofday(&start, NULL);

    cudaMalloc(&cuda_in, sizeof(unsigned char) * n * width * height);
    cudaMalloc(&cuda_out, sizeof(unsigned char) * n * width * height);

    cudaMemcpy(cuda_in, in, sizeof(unsigned char) * n * width * height, cudaMemcpyHostToDevice);
    cudaEventRecord(start);

    process << < grid_size, treads_size >> > (grid_size, treads_size, n, height, width, cuda_in, cuda_out);

    cudaEventRecord(stop);

    cudaMemcpy(out, cuda_out, sizeof(unsigned char) * n * width * height, cudaMemcpyDeviceToHost);
    cudaEventSynchronize(stop);

    cudaFree(cuda_in);
    cudaFree(cuda_out);

    float milliseconds = 0;
    cudaEventElapsedTime(&milliseconds, start, stop);
    printf("\n\nTIME %f\n\n", milliseconds);

        /*gettimeofday(&end, NULL);
        printf("Sending result...\n");
        double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + 
         end.tv_usec - start.tv_usec) / 1.e6;
	printf("\nElapsed: %lf ms\n", delta);*/

    for (size_t i = 0; i < n; i++) {
        write(sockfd, out + i * height * width, sizeof(unsigned char) * height * width);
    }
        
    write(sockfd, &milliseconds, sizeof(milliseconds));
    printf("Result send.\n");
    free(in);
    free(out);
    close(sockfd); 
    
    return 0;
} 
