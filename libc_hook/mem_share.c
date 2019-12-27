#include "../C_util/util.h"
#include "../C_util/util.c"
#include "../config.h"
#include "mem_share.h"

static int *data_mem;
static int *inf_mem;
static int is_init = 0;

void mssleep(int const time_in_ms) {   
    struct timespec time;
    struct timespec time_buf;
 
    int ret = -1;
    time.tv_sec = (time_in_ms / 1000);
    time.tv_nsec = (1000 * 1000 * (time_in_ms % 1000));
 
    time_buf = time;
    while(1) {
        time = time_buf;
        ret = nanosleep(&time, &time_buf);
        if((ret < 0)) {
			continue;
		}
        else {
            break;
        }  
    }
    return;
}

void mem_share_init() {
    size_t share_mem_size = sizeof(int) * data_mem_size;
    size_t inf_size = sizeof(int) * 3;
    data_mem = (int *)open_share_mem(share_mem_name, share_mem_size);
    inf_mem = (int *)open_share_mem(share_mem_inf_name, inf_size);
}

void push_data(int Pid) {
    while (inf_mem[1] == data_mem_size - 1) {
        printf("need more buffer %d\n", inf_mem[1]);
        mssleep(5);
    }
    data_mem[inf_mem[1]] = Pid;
    inf_mem[1] ++;
    inf_mem[0] = 1;
}