#include <server.h>

/* Find out file size */

int getFileSize(int filefd) {
    struct stat st;
    if (fstat(filefd, &st) < 0) {
        perror("ERROR get file size");
    }
    return st.st_size;
}