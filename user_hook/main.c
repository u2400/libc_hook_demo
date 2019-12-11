#include <stdio.h>
#include <zconf.h>
#include "get_proc_inf.h"



//void get_stat(struct proc_inf inf, char* Path) {
//    struct proc_stat stat = get_proc_stat(-1);
//    if (lstat(Path, &buf) == -1) {}
//
//}

int main() {
    struct proc_inf inf = get_porc_inf();
    char dir[PATH_MAX] = {0};
    printf("%s %lu %lu", inf.Cmdline, inf.start_time, inf.run_time);
}