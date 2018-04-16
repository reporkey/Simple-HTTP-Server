#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>

static const int MAX_BUF_CHAR_NO = 4196;
static const int MAX_HEAD_LEN = 4196;
static const char* HEADER_200 = "HTTP/1.0 200 OK\r\n";
static const char* HEADER_404 = "HTTP/1.0 404 NOT FOUND\r\n";
static const char* HEADER_HTML = "Content-Type: text/html\r\n\r\n";
static const char* HEADER_CSS = "Content-Type: text/css\r\n\r\n";
static const char* HEADER_JPEG = "Content-Type: image/jpeg\n\r\n";
static const char* HEADER_JS = "Content-Type: text/javascript\n\r\n";

/*REMEMBER TO CHANGE BACK*/
/*REMEMBER TO CHANGE BACK*/
/*REMEMBER TO CHANGE BACK*/
//static const char* DOMAIN = "/home/ubuntu/comp30023/ass1/test";
//static const char* DOMAIN = "/home/comp30023/website";


typedef struct {
    int cli_sockfd;
    char* domain;
}args_T;

void* acceptClient(void *param);
void router(char buffer[], int cli_sockfd, char* domain);
int getFileSize(int filefd);
char* findType(char* abspath);