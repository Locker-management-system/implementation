#include <stdio.h>
#include <unistd.h>

int readLine(int fd, char* str)
{
    int n;
    do {
        n = read(fd, str, 1);
    } while(n > 0 && *str++ != NULL);
    return(n > 0);
}

int readFile(int fd, char* str)
{
    int n,i,cnt;
    do {
        n = read(fd, str, 1);
        if(str[i++] == '.') cnt = 1;
    } while(n > 0 && *str++ != NULL);
    if(cnt == 1)
        return(n > 0);

    return 0;
}


int readIndex(int fd, int index){

    int x;
    do{
        x = read(fd, (char*) index, 1);
    } while(x > 0);
    return (x > 0 );
}

int readCheck(int fd, char* check) {
    int c;
    c = read(fd, check, 1);
    return c;
}
