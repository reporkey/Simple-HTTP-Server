#include <server.h>

/* Find out file size */

int getFileSize(int filefd) {
    struct stat st;
    if (fstat(filefd, &st) < 0) {
        perror("ERROR get file size");
    }
    return st.st_size;
}

char* findType(char* abspath){
    char *type = malloc(MAX_HEAD_LEN * sizeof(char));
    strcpy(type, strrchr(abspath, '.'));

    if (strcmp(type, ".html") == 0){
        strcpy(type, HEADER_HTML);
    }
    if (strcmp(type, ".css") == 0){
        strcpy(type, HEADER_CSS);
    }
    if (strcmp(type, ".jpg") == 0){
        strcpy(type, HEADER_JPEG);
    }

    if (strcmp(type, ".js") == 0){
        strcpy(type, HEADER_JS);
    }
    return type;
}