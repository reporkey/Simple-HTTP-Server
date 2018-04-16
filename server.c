/*
 To compile: gcc server.c -o server -Wall -lpthread
 To run: ./server portno
*/

#include<server.h>

int main(int argc, char *argv[]) {
    int portno, sockfd;
	struct sockaddr_in serv_addr;
    int cli_sockfd;
    struct sockaddr_in cli_addr;
    socklen_t clilen = sizeof(cli_addr);

    if (argc < 3){
        printf("ERROR, no port provided\n");
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
    char* domain = malloc(strlen(argv[2]) * sizeof(char));
    strcpy(domain, argv[2]);

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

	listen(sockfd, 10);

    pthread_t tid;
    while (1){

        /* Accept client*/

        cli_sockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (cli_sockfd < 0) {
            perror("ERROR on accept");
            exit(1);
        }

        /* Package clients */

        args_T *args = malloc(sizeof(args_T) + 1);
        args->cli_sockfd = cli_sockfd;
        args->domain = domain;

        /* Create threads, pass client to each thread*/

        if (pthread_create(&tid, NULL, acceptClient, args)) {
            exit(1);
        }
    }

    /* close socket */
    pthread_join(tid, NULL);

	close(sockfd);
	return 0;
}


void* acceptClient(void *args) {

    int n;
    char buffer[MAX_BUF_CHAR_NO];
    bzero(buffer,MAX_BUF_CHAR_NO);

    args_T *tempargs = args;
    int cli_sockfd = tempargs->cli_sockfd;
    char* domain = tempargs->domain;

    /* Read characters from the connection,
    then process */

    n = read(cli_sockfd, buffer, MAX_BUF_CHAR_NO-1);
    if (n < 0) {
        perror("ERROR reading from socket");
        exit(1);
    }

    /* Place analise the requests*/

    router(buffer, cli_sockfd, domain);


    close(cli_sockfd);
    free(tempargs);
    return 0;
}

void router(char buffer[], int cli_sockfd, char* domain){

    char* rltpath;
    char* abspath = (char *) malloc(MAX_BUF_CHAR_NO * sizeof(char));
    memset(abspath, 0, MAX_BUF_CHAR_NO);
    int filefd;
    int n;

    /* Analyse the domain and the path from the request*/

    rltpath = strtok(buffer, " ");
    rltpath = strtok(NULL, " ");

    if (strncmp(rltpath, domain, strlen(domain)) != 0) {
        abspath = strcat(abspath, domain);
        abspath = strcat(abspath, rltpath);
    }else{
        abspath = strcpy(abspath, rltpath);
    }

    /* Get the type*/
    char* type = findType(abspath);

    if (access(abspath, R_OK) == 0){

        filefd = open(abspath, O_RDONLY);
        if (filefd < 0){
            perror("ERROR open file");
            exit(1);
        }

        /* Send the 200 header*/

        char* header = malloc(MAX_HEAD_LEN * sizeof(char));
        strcpy(header, HEADER_200);
        strcat(header, type);
        n = write(cli_sockfd, header, strlen(header));
        if (n < 0){
            perror("ERROR send header");
        }

        /* Send file */
        int fsize = getFileSize(filefd);
        n = sendfile(cli_sockfd, filefd, NULL, fsize);
        if (n < 0){
            perror("ERROR send file");
        }
        free(header);
    }else{

        /* Send the 404 header*/

        char* header = malloc(MAX_HEAD_LEN * sizeof(char));
        strcpy(header, HEADER_404);
        n = write(cli_sockfd, header, strlen(header));
        if (n < 0){
            perror("ERROR send header");
        }
        free(header);
    }
    close(filefd);
    free(type);
    free(abspath);
    return;
}