/*
 To compile: gcc server.c -o server -Wall -lpthread
 To run: ./server portno
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>

#define THREAD_NO 5
#define MAX_BUF_CHAR_NO 2048
#defien HEADER_200 "HTTP/1.0 200 OK\n"
#defien HEADER_400 "HTTP/1.0 400 OK\n"
#define HEADER_HTML "Content-Type: text/html\n\n"

/*REMEMBER TO CHANGE BACK*/
/*REMEMBER TO CHANGE BACK*/
/*REMEMBER TO CHANGE BACK*/
#define DOMAIN "/home/ubuntu/comp30023/ass1/test"

typedef struct {
    int cli_sockfd;
}args_T;

void* acceptClient(void *param);
void mainRouter(char buffer[], int cli_sockfd);

int main(int argc, char *argv[]) {
    int portno, sockfd;
	struct sockaddr_in serv_addr;
    int cli_sockfd;
    struct sockaddr_in cli_addr;
    socklen_t clilen;

    if (argc < 2)
    {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }

	 /* Create TCP socket
	  * int socket(int domain, int type, int protocol);
	  * AF_INET: IPv4 Internet protocols
	  * SOCK_STREAM:Provides sequenced, reliable, two-way, connection-based byte streams.
	  * An out-of-band data transmission mechanism may be supported.*/
	 
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
		exit(1); 
    }
    portno = atoi(argv[1]);
	 
	 //rewrite all bits to 0
    
    bzero((char *) &serv_addr, sizeof(serv_addr));
	
	/* Create address we're going to listen on (given port number)
	 - converted to network byte order & any IP address for 
	 this machine */

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);  // store in machine-neutral format

	 /* Bind address to the socket */

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		perror("ERROR on binding");
		exit(1);
	}
	
	/* Listen on socket - means we're ready to accept connections - 
	 incoming connection requests will be queued */

	listen(sockfd,5);

    /* Accept a connection - block until a connection is ready to
     be accepted. Get back a new file descriptor to communicate on. */


    pthread_t tid;


    for (int i = 0; i < THREAD_NO; ++i) {

        /* Accept client*/

        cli_sockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (cli_sockfd < 0) {
            perror("ERROR on accept");
            exit(1);
        }

        /* Package clients */

        args_T *args = malloc(sizeof(args_T));
        args->cli_sockfd = cli_sockfd;

        /* Create threads, pass client to each thread*/

        if (pthread_create(&tid, NULL, acceptClient, args)) {
            printf("\n Error creating thread %d", i);
            exit(1);
        }
    }

    pthread_join(tid, NULL);
    pthread_exit(NULL);

    /* close socket */
	close(sockfd);
	return 0;
}


void* acceptClient(void *args) {

    int n;
    char buffer[MAX_BUF_CHAR_NO];
    bzero(buffer,MAX_BUF_CHAR_NO);

    args_T *newargs = args;
    int cli_sockfd = newargs->cli_sockfd;

    /* Read characters from the connection,
    then process */

    n = read(cli_sockfd, buffer, MAX_BUF_CHAR_NO-1);
    if (n < 0) {
        perror("ERROR reading from socket");
        exit(1);
    }

    /* Place analise the requests*/

    mainRouter(buffer, cli_sockfd);


    close(cli_sockfd);
    free(newargs);
    return 0;
}

void mainRouter(char buffer[], int cli_sockfd){

    char* rltpath;
    char* abspath = malloc(MAX_BUF_CHAR_NO* sizeof(char));
    int filefd;
    int n;

    /* Analyse the domain and the path from the request*/

    rltpath = strtok(buffer, " ");
    rltpath = strtok(NULL, " ");

    if (strncmp(rltpath, DOMAIN, 23) != 0) {
        abspath = strcat(abspath, DOMAIN);
        abspath = strcat(abspath, rltpath);
    }else{
        abspath = strcpy(abspath, rltpath);
    }
    printf("abspath is : %s\n\n", abspath);

    if (access(abspath, R_OK) == 0){
        printf("FOUND FILE at: %s\n", abspath);
        filefd = open(abspath, O_RDONLY);
        if (filefd < 0){
            perror("ERROR open file");
            exit(1);
        }

        /* Send the 200 header*/

        n = write(cli_sockfd, "HTTP/1.0 200 OK\nContent-Type: text/html\n\n", 41);
        printf("%lu\n", sizeof("HTTP/1.0 200 OK\nContent-Type: text/html\n\n"));
        if (n < 0){
            perror("ERROR send header");
        }

        /* Send file */

        n = sendfile(cli_sockfd, filefd, NULL, 100);
        if (n < 0){
            perror("ERROR send file");
        }

        close(filefd);
    }else{
        printf("NOT FOUND FILE at: %s\n", abspath);
        n = send(cli_sockfd, "HTTP/1.0 400 OK\nContent-Type: text/html\n", 40, 0);
    }
}

