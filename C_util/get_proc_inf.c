//
// Created by toor on 2019-11-14.
//


#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <asm/param.h>
#include <sys/sysinfo.h>
#include <zconf.h>
#include <malloc.h>
#include <libnet.h>
#include <time.h>

//获取指定Pid进程的/proc/self/stat
struct proc_stat get_proc_stat(int Pid) {
    FILE *f = NULL;
    struct proc_stat stat = {0};
    char tmp[100] = "0";
    stat.comm = tmp;

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

char *get_proc_env(int Pid) {
    char *env;
    char *stat_path = format_path(Pid, "/proc/%s/environ");
    env = read_file(stat_path);
    return env;
}


char *get_proc_path(int Pid) {
    char stat_path[20];
    char* pstat_path = stat_path;
    char* dir = (char *)calloc((size_t)PATH_MAX, '\0');
    if (Pid != -1) {
        sprintf(stat_path, "/proc/%d/exe", Pid);
    } else {
        pstat_path = "/proc/self/exe";
    }

    readlink(pstat_path, dir, PATH_MAX);
    return dir;
}

char* get_proc_cmdline(int Pid) {
    FILE* f;
    char stat_path[100] = {0};
    char* pstat_path = stat_path;

    if (Pid != -1) {
        sprintf(stat_path, "/proc/%d/cmdline", Pid);
    } else {
        pstat_path = "/proc/self/cmdline";
    }

    if ((f = fopen(pstat_path, "r")) == NULL) {
        printf("open file error");
        return "";
    }
    char* pcmdline = (char *)malloc((size_t)get_file_length(f));
    char ch;
    ch = (char)getc(f);
    for (int i = 0;ch != EOF; i++ ) {
        *(pcmdline + i) = ch;
        ch = (char)getc(f);

        // /proc/self/cmdline中所有的空格/回车都会被替换为'\0'所以需要自己转换回来
        if ((int)ch == 0) {
            ch = ' ';
        }
    }
    return pcmdline;
}

struct stat* get_file_stat(char* Path) {
    struct stat* file_stat = (struct stat*)malloc((sizeof(struct stat)));
    stat(Path, file_stat);
    return file_stat;
}

char** get_proc_fd(int Pid) {
    char** tmp = (char **)malloc(3*sizeof(char *));
    char read_path[PATH_MAX];

    *(tmp + 0) = (char *)malloc((PATH_MAX));
    *(tmp + 1) = (char *)malloc((PATH_MAX));
    *(tmp + 2) = (char *)malloc((PATH_MAX));

    char stat_path[100] = {0};
    char* pstat_path = stat_path;
    if (Pid != -1) {
        sprintf(stat_path, "/proc/%d/fd", Pid);
    } else {
        pstat_path = "/proc/self/fd";
    }

    sprintf(read_path, "%s/0", pstat_path);
    readlink(pstat_path, *(tmp + 0), PATH_MAX);

    sprintf(read_path, "%s/1", pstat_path);
    readlink(pstat_path, *(tmp + 1), PATH_MAX);

    sprintf(read_path, "%s/2", pstat_path);
    readlink(pstat_path, *(tmp + 2), PATH_MAX);
    return tmp;
}

struct proc_inf get_proc_inf(int Pid) {
    struct proc_inf inf = {0};
    struct proc_stat stat = get_proc_stat(Pid);
    inf.pid = stat.pid;
    inf.ppid = stat.ppid;
    inf.sid = stat.session;
    inf.pgid = stat.pgid;
    inf.ENV = get_proc_env(stat.pid);
    time_t t;
    time(&t);
    inf.sys_time = (long unsigned int)t;
    inf.start_time = jiffies_to_unix_time(stat.starttime);
    inf.run_time = inf.sys_time - inf.start_time;
    inf.Path = get_proc_path(-1);
    inf.Cmdline = get_proc_cmdline(-1);
    inf.uid = getuid();
    inf.Euid = geteuid();
    inf.egid = getegid();
    inf.Gid = getgid();

    struct stat* file_stat = get_file_stat(inf.Path);
    inf.Owner_gid = file_stat->st_gid;
    inf.Owner_uid = file_stat->st_uid;
    inf.Modify_time = file_stat->st_mtim.tv_sec;
    inf.mode = convertDecimalToOctal(file_stat->st_mode);
    inf.fd = get_proc_fd(inf.pid);
    return inf;
}