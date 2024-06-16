#include <stdio.h>
#include <unistd.h>

readLine(int fd, char* str)
{
    int n;
    do {
        n = read(fd, str, 1);
    } while(n > 0 && *str++ != NULL);
    return(n > 0);
}


readIndex(int fd, int index){

    int x;
    x = read(fd, index, 1);
    return x;
}

