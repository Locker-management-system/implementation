#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<fcntl.h>
#include"cabinet_manager.h"

int get_cabinet_fd(){
    int cabinet_fd;
    if((cabinet_fd = open(CABINETDB, O_WRONLY|O_CREAT|O_EXCL, 0640)) == -1){
        printf("cabinet_db 연결 오류\n");
        return cabinet_fd;
    }

    return 1;
}

int set_cabinet(Cabinet cabinet){

}

int is_cabinet_empty(){
    Cabinet record;
    int fd = get_cabinet_fd();
    lseek(fd, START_CABINET_ID*sizeof(record), SEEK_SET);
    if(!(read(fd, &record, sizeof(record)) > 0)){   //레코드 존재 x
        for(int i = START_CABINET_ID; i <= MAX_CABINET_SIZE; i++){
            clear_cabinet(i);
        }
    }
}

Cabinet get_cabinet(int index);
int set_file(int cabinet_index, char* file_path);
int delete_file(int cabinet_index);
int clear_cabinet(int cabinet_index);