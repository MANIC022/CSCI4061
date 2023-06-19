#include <fcntl.h>

int main(){

    //int creat(const char *path, mode_t mode);
    int fd;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    char *pathname = "del_me";
    fd = creat(pathname, mode);

    return 0;

}