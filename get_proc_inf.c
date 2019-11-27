//
// Created by toor on 2019-11-14.
//

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <asm/param.h>
#include <sys/sysinfo.h>
#include "get_proc_inf.h"

long unsigned int jiffies_to_unix_time(long unsigned int jiffies) {
    struct sysinfo info;
    time_t t;
    time(&t);
    sysinfo(&info);
    long unsigned int sys_start_time = (long unsigned int)info.uptime;
    printf("%lu\n", sys_start_time);
    return (t - sys_start_time + (jiffies/HZ));
}

//获取指定Pid进程的/proc/self/stat
struct proc_stat get_proc_stat(int Pid) {
    FILE *f = NULL;
    struct proc_stat stat = {0};
    char tmp[100] = "0";
    stat.comm = tmp;
    char stat_path[20];
    char* pstat_path = stat_path;

    if (Pid != -1) {
        sprintf(stat_path, "/proc/%d/stat", Pid);
    } else {
        pstat_path = "/proc/self/stat";
    }

    if ((f = fopen(pstat_path, "r")) == NULL) {
        printf("open file error");
        return stat;
    }

    fscanf(f, "%d ", &stat.pid);
    fscanf(f, "(%100s ", stat.comm);
    tmp[strlen(tmp)-1] = '\0';
    fscanf(f, "%c ", &stat.state);
    fscanf(f, "%d ", &stat.ppid);
    fscanf(f, "%d ", &stat.pgid);

    fscanf (
            f,
            "%d %d %d %u %lu %lu %lu %lu %lu %lu %ld %ld %ld %ld %ld %ld %llu %lu %ld %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %d %d %u %u %llu %lu %ld %lu %lu %lu %lu %lu %lu %lu %d",
            &stat.session, &stat.tty_nr, &stat.tpgid, &stat.flags, &stat.minflt,
            &stat.cminflt, &stat.majflt, &stat.cmajflt, &stat.utime, &stat.stime,
            &stat.cutime, &stat.cstime, &stat.priority, &stat.nice, &stat.num_threads,
            &stat.itrealvalue, &stat.starttime, &stat.vsize, &stat.rss, &stat.rsslim,
            &stat.startcode, &stat.endcode, &stat.startstack, &stat.kstkesp, &stat.kstkeip,
            &stat.signal, &stat.blocked, &stat.sigignore, &stat.sigcatch, &stat.wchan,
            &stat.nswap, &stat.cnswap, &stat.exit_signal, &stat.processor, &stat.rt_priority,
            &stat.policy, &stat.delayacct_blkio_ticks, &stat.guest_time, &stat.cguest_time, &stat.start_data,
            &stat.end_data, &stat.start_brk, &stat.arg_start, &stat.arg_end, &stat.env_start,
            &stat.env_end, &stat.exit_code
    );
    fclose(f);
    return stat;
}

//获取全部的当前进程的环境变量
extern char **environ;
char **get_all_env(void) {
    char **p = environ;
    return p;
}

struct proc_inf get_porc_inf(void) {
    struct proc_inf inf = {0};
    struct proc_stat stat = get_proc_stat(-1);
    inf.pid = stat.pid;
    inf.ppid = stat.ppid;
    inf.sid = stat.session;
    inf.pgid = stat.pgid;
    inf.ENV = get_all_env();
    time_t t;
    time(&t);
    inf.sys_time = (long unsigned int)t;
    inf.start_time = jiffies_to_unix_time(stat.starttime);
    inf.run_time = inf.sys_time - inf.start_time;
    return inf;
}