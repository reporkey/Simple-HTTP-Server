/* A simple server in the internet domain using TCP
The port number is passed as an argument 

 To compile: gcc server.c -o server 
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

#define PORT_NO 8081
#define THREAD_NO 5
#define MAN_BUF_CHAR_NO 2048

typedef struct {
    int sockfd;
}args_T;

void* acceptClient(void *param);
void mainRouter(char buffer[], int cli_sockfd);

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
	serv_addr.sin_port = htons(PORT_NO);  // store in machine-neutral format

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
    int n;

    char buffer[MAN_BUF_CHAR_NO];
    bzero(buffer,MAN_BUF_CHAR_NO);

    args_T *newargs = args;
    int sockfd = newargs->sockfd;

    clilen = sizeof(cli_addr);
    cli_sockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (cli_sockfd < 0) {
        perror("ERROR on accept");
        exit(1);
    }


    /* Read characters from the connection,
    then process */

    n = read(cli_sockfd, buffer, MAN_BUF_CHAR_NO-1);
    if (n < 0) {
        perror("ERROR reading from socket");
        exit(1);
    }

    /* Place analise the requests*/
    mainRouter(buffer, cli_sockfd);


    close(cli_sockfd);
    return 0;
}

void mainRouter(char buffer[], int cli_sockfd){
    printf("%s\n\n", buffer);

    char *path;
    int filefd;
    int n;

    /* Get the domain and the path from the request*/

    path = strstr(buffer, "/home/comp30023/website");
    path = strtok(path, " ");

    if (access(path, R_OK) == 0){
        printf("FOUND FILE at: %s", path);

        filefd = open(path, O_RDONLY);
        if (filefd < 0){
            perror("ERROR open file");
        }
        n = sendfile(cli_sockfd, filefd, NULL, 175000);
        if (n < 0){
            perror("ERROR send file");
        }

        close(filefd);
    }else{
        printf("NOT FOUND FILE at: %s\n", path);
    }



}


//void chackvalid(){
//    /* Check domain validity*/
//    path = strtok(domain, "/");
//    if (strcmp(path, "home")){
//        printf("Domain is not valid.\n");
//        exit(1);
//    }
//    printf("pd is: %s\n\n", path);
//    path = strtok(NULL, "/");
//    if (strcmp(path, "comp30023")){
//        printf("Domain is not valid.\n");
//        exit(1);
//    }
//    printf("pd is: %s\n\n", path);
//    path = strtok(NULL, "/");
//    if (strcmp(path, "website")){
//        printf("Domain is not valid.\n");
//        exit(1);
//    }
//    printf("domain is: %s\n\n", domain);
//    printf("pd is: %s\n\n", path);
//}










