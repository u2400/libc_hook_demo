#include <stdio.h>
#include <zconf.h>
#include "get_proc_inf.h"

int main() {
//    struct proc_inf inf = get_proc_inf(-1);
//    char dir[PATH_MAX] = {0};
//    char *env = get_proc_env(-1);
    test_get_file_lenth("/proc/self/environ");
//    printf("%s", env);
//    printf("%s %lu %lu", inf.Cmdline, inf.start_time, inf.run_time);
}