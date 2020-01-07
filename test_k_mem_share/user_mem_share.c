/*
 * @Date: 2020-01-03 05:07:30
 * @LastEditors  : u2400
 * @LastEditTime : 2020-01-07 04:04:00
 */
#include "../config.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

void *open_k_share_mem(char *name, size_t size) {
    char *path = NULL;
    sprintf(path, "/dev/%s", name);
    
    int f = open(path, O_RDWR);

    if (f < 0) {
        printf("fopen error\n");
        return NULL;
    }
    void* ptr = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, f, 0);
    
    if (ptr == NULL) {
        printf("open share mem error");
    }
    close(f);
    return ptr;
}