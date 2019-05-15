#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <stdio.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h>
#include <time.h> 
#include <mpi.h>
#define PORT 8080 
#define SA struct sockaddr 

int create() {
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
    
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
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

void process(int n, int height, int width, u_char *in) {

    u_char *out = malloc(sizeof(u_char) * n * width * height);
    printf("Start calculations\n");
    int i, j, k;
    for(i = 0; i < n; i++) {     
        for(j = 0; j < height; j++) {
            for(k = 0; k < width; k++) {
                int current = i * width * height + j * width + k;
                //printf("i = %d, j= %d, value = %x \n", i, j, out[current]);
                if (k + j < width) {
                    out[current] = in[current + j];
                } else {
                    out[current] = in[current + j - width];
                }
            }
        }
    }

    in = out;

//Тестовый вывод
    for(size_t i = 0; i < n; i++)
        {
            for(size_t j = 0; j < height * width; j++) {
	    	if (j % width == 0) {
                    printf("\n");
                }
                if (j % height * width == 0) {
                    printf("\n");
                }
		printf("\t%x", in[i * height * width + j]);
            }
        }


    printf("Calculations complete\n");
}
  
// Driver function 
int main(int argc, char **argv) 
{ 
    int sockfd;
    int rank, size, rc;
    int n, width, height;
    u_char *in = NULL;
    u_char *out = NULL;
    u_char buff[height * width];
    
    if ((rc = MPI_Init(&argc, &argv)) != MPI_SUCCESS) {
        fprintf(stderr, "Error starting MPI program. Terminating.\n");
        MPI_Abort(MPI_COMM_WORLD, rc);
    }
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {

    sockfd = create();
    
        //int n = 0;
        read(sockfd, &n, sizeof(n));
        //int width = 0;
        read(sockfd, &width, sizeof(width));
        //int height = 0;
        read(sockfd, &height, sizeof(height));
        n = ntohl(n);
        width = ntohl(width);
        height = ntohl(height);
        

        printf("Received header:\n Number of matrices = %d\n Width of each = %d\n Height of each = %d\n", n, width, height);

        in = malloc(sizeof(u_char) * n * width * height);
        out = malloc(sizeof(u_char) * n * width * height);

        

        for (size_t i = 0; i < n; i++) {
            read(sockfd, in + i * height * width, sizeof(buff));
        }

	//struct timeval start, end;
	//gettimeofday(&start, NULL);
}

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&width, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&height, 1, MPI_INT, 0, MPI_COMM_WORLD);
    int p = n / size;
    printf("process %d of %d всего матриц %ld  количество матриц на поток %ld\n", rank, size, n, p);

   printf("%d \n", height * width * p);
	//получить данные от рута
    u_char *recvbuf = (u_char *) malloc(height * width * sizeof(u_char) * p);
    MPI_Scatter(in, height * width * p, MPI_UNSIGNED_CHAR, recvbuf, height * width * p,
                MPI_UNSIGNED_CHAR, 0,
                MPI_COMM_WORLD);

    //что то сделать с данными
    process(p, height, width, recvbuf);

    MPI_Gather(recvbuf, height * width * p, MPI_UNSIGNED_CHAR, out, height * width * p,
               MPI_UNSIGNED_CHAR, 0,
               MPI_COMM_WORLD);

    printf("\nGather complete!\n");
	//gettimeofday(&end, NULL);

	//double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + 
        // end.tv_usec - start.tv_usec) / 1.e6;
	//printf("\nElapsed: %lf ms\n", delta);


if (rank == 0) {
        printf("Sending result...\n");

        for (size_t i = 0; i < n; i++) {
            write(sockfd, out + i * height * width, sizeof(buff));
        }

	//write(sockfd, &delta, sizeof(delta));

	printf("Result send.\n");
        free(in);
        free(out);
close(sockfd); 
}
    
MPI_Finalize();
} 
