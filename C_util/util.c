//
// Created by user on 12/17/19.
//

#include "../config.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/sysinfo.h>
#include <sys/param.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

long unsigned int jiffies_to_unix_time(long unsigned int jiffies) {
    struct sysinfo info;
    time_t t;
    time(&t);
    sysinfo(&info);
    long unsigned int sys_start_time = (long unsigned int)info.uptime;
    printf("%lu\n", sys_start_time);
    return (t - sys_start_time + (jiffies/HZ));
}

long get_file_length(FILE* f) {
    fseek(f,0L,SEEK_SET);
    char ch;
    ch = (char)getc(f);
    long i;
    for (i = 0;ch != EOF; i++ ) {
        ch = (char)getc(f);
    }
    i++;
    fseek(f,0L,SEEK_SET);
    return i;
}

int convertDecimalToOctal(int decimalNumber) {
    int octalNumber = 0, i = 1;
    while (decimalNumber != 0)
    {
        octalNumber += (decimalNumber % 8) * i;
        decimalNumber /= 8;
        i *= 10;
    }
    return octalNumber;
}

char *format_path(int Pid, char *path) {
    char s_Pid[10] = {'\0'};
    char *ended_path = calloc('\0' ,500);

    if(Pid != -1) {
        sprintf(s_Pid, "%d", Pid);
        sprintf(ended_path, path, Pid);
    } else {
        sprintf(ended_path, path, "self");
    }
    return ended_path;
}

char *read_file(char *path) {
    char ch;
    FILE* f;
    long length;
    char *file_content;

    if ((f = fopen(path, "r")) == NULL) {
        printf("open file error");
        return "";
    }

    length = get_file_length(f);
    printf("%ld",length);
    file_content = (char *)calloc((size_t)length, '\0');

    ch = (char)getc(f);
    for (int i = 0;ch != EOF; i++ ) {
        *(file_content + i) = ch;
        ch = (char)getc(f);

        if ((int)ch == 0) {
            ch = '\n';
        }
    }

    fclose(f);
    return file_content;
}

void error(char *mes) {
    printf("%s", mes);
}

void *new_share_mem(char *name, size_t size) {
    int f = shm_open(name, O_RDWR|O_CREAT, 0);
    ftruncate(f, size);

    void* ptr = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, f, 0);
    close(f);
    if (ptr == NULL) {
        printf("new share mem errror");
    }
    return ptr;
}

void *free_share_mem(char *name) {
    shm_unlink(name);
}

void *open_share_mem(char *name, size_t size) {
    int f = shm_open(name, O_RDWR, 0);
    void* ptr = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, f, 0);
    close(f);

    if (ptr == NULL) {
        printf("open share mem error");
    }
    return ptr;
}
