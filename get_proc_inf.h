//
// Created by toor on 19-11-16.
//

#ifndef LIBC_PRELOAD_GET_PROC_INF_H
#define LIBC_PRELOAD_GET_PROC_INF_H

struct proc_stat {
    int pid;
    char* comm;
    char state;
    int ppid;
    int pgid;
    int session;
    int tty_nr;
    int tpgid;
    unsigned int flags;
    long unsigned int minflt;
    long unsigned int cminflt;
    long unsigned int majflt;
    long unsigned int cmajflt;
    long unsigned int utime;
    long unsigned int stime;
    long int cutime;
    long int cstime;
    long int priority;
    long int nice;
    long int num_threads;
    long int itrealvalue;
    long long unsigned int starttime;
    long unsigned int vsize;
    long int rss;
    long unsigned int rsslim;
    long unsigned int startcode;
    long unsigned int endcode;
    long unsigned int startstack;
    long unsigned int kstkesp;
    long unsigned int kstkeip;
    long unsigned int signal;
    long unsigned int blocked;
    long unsigned int sigignore;
    long unsigned int sigcatch;
    long unsigned int wchan;
    long unsigned int nswap;
    long unsigned int cnswap;
    int exit_signal;
    int processor;
    unsigned int rt_priority;
    unsigned int policy;
    long long unsigned int delayacct_blkio_ticks;
    long unsigned int guest_time;
    long int cguest_time;
    long unsigned int start_data;
    long unsigned int end_data;
    long unsigned int start_brk;
    long unsigned int arg_start;
    long unsigned int arg_end;
    long unsigned int env_start;
    long unsigned int env_end;
    int exit_code;
};

struct proc_inf {
    char* Path;     //ok
    char** ENV;     //ok
    char* Cmdline;
    char* pcmdline;
    char* ppath;
    char* name;
    int pid;        //ok
    int ppid;       //ok
    int pgid;
    int uid;
    int Euid;
    int Gid;
    int egid;
    int sid;
    int mode;
    int Owner_uid;
    int Owner_gid;
    int Create_time;
    int Modify_time;
    //ok
    long unsigned int  start_time; //proc run time
    long unsigned int run_time; //proc runed time
    long unsigned int sys_time;
};

struct proc_stat get_proc_stat(int Pid);
struct proc_inf get_porc_inf(void);
char **get_all_env(void);
void get_time(void);

#endif //LIBC_PRELOAD_GET_PROC_INF_H
