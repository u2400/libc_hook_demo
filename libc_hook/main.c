#include <stdio.h>
#include "../C_util/util.h"
#include "../C_util/util.c"
#include "../config.h"



int main() {
    size_t share_mem_size = sizeof(int) * 21;
    size_t inf_size = sizeof(int) * 3;
    int *mem = (int *)open_share_mem(share_mem_name, share_mem_size);
    int *mem_inf = (int *)open_share_mem(share_mem_inf_name, inf_size);
    printf("%d\n", mem[0]);
    mem[3] = 100;
    mem_inf[0] = 1;
}