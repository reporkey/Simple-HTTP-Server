/* A simple server in the internet domain using TCP
The port number is passed as an argument 

 To compile: gcc server.c -o server 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define PORT_NO 8080
#define THREAD_NO 5
#define MAN_BUF_CHAR_NO 2048

typedef struct {
    int sockfd;
}args_T;

void* acceptClient(void *param);
void mainRouter(char buffer[]);
char* readRequest(char buffer[]);

int main(int argc, char *argv[]) {
    int sockfd;
	struct sockaddr_in serv_addr;

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
	 
	 //rewrite all bits to 0
    
    bzero((char *) &serv_addr, sizeof(serv_addr));
	
	/* Create address we're going to listen on (given port number)
	 - converted to network byte order & any IP address for 
	 this machine */

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(8091);  // store in machine-neutral format

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

    args_T *args = malloc(sizeof(args_T));
    args->sockfd = sockfd;
    pthread_t tid;

    for (int i = 0; i < 1; ++i) {
        if (pthread_create(&tid, NULL, acceptClient, args)) {
            printf("\n Error creating thread %d", i);
            exit(1);
        }
    }
    pthread_join(tid, NULL);
    pthread_exit(NULL);

    /* close socket */
	close(sockfd);
	free(args);
	return 0;
}


void* acceptClient(void *args) {
    
    int cli_sockfd;
    struct sockaddr_in cli_addr;
    socklen_t clilen;
    char buffer[MAN_BUF_CHAR_NO];
    int n;

    args_T *newargs = args;
    int sockfd = newargs->sockfd;

    clilen = sizeof(cli_addr);
    cli_sockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (cli_sockfd < 0) {
        perror("ERROR on accept");
        exit(1);
    }

    bzero(buffer,MAN_BUF_CHAR_NO);

    /* Read characters from the connection,
    then process */

    n = read(cli_sockfd, buffer, MAN_BUF_CHAR_NO-1);
    if (n < 0) {
        perror("ERROR reading from socket");
        exit(1);
    }

    /* Place analise the requests*/
    mainRouter(buffer);


    close(cli_sockfd);
    return 0;
}

void mainRouter(char buffer[]){
    printf("%s\n//\n", buffer);
    printf("%s", buffer+ 4*sizeof(char));
    printf("sizeof buff: %d\n", buffer[4] == '\n');
//    readRequest(buffer);



}

char* readRequest(char buffer[]){

    char path[MAN_BUF_CHAR_NO];

    /* get first line in request*/
    for (int i=0; ; i++){
        if (buffer[i] != '\n'){
            path[i] = buffer[i];
        }else{
            break;
        }
    }

    /* separata path*/


    return path;
}












