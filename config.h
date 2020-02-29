/*
 * @Date: 2019-12-26 19:30:56
 * @LastEditors  : u2400
 * @LastEditTime : 2020-02-01 02:58:26
 */
#ifndef CONFIG_H
#define CONFIG_H

typedef struct {
    char* Path;
    char* ENV;
    char* Cmdline;
    char* Pcmdline;
    char* PPath;
    char* Name;
    int Pid;
    int PPid;
    int Pgid;
    int Uid;
    int Euid;
    int Gid;
    int Egid;
    int Sid;
    int Mode;
    int Owner_uid;
    int Owner_gid;
    int Create_time;
    long Modify_time;
    long unsigned int Start_time; //proc run time
    long unsigned int Run_time; //proc runed time
    long unsigned int Sys_time;
    char** Fd;
} proc_inf;

#define share_mem_name "secret_key1"
#define share_mem_inf_name "secret_key2"
#define DEVICE_NAME "secret_key1"
#define CLASS_NAME  "secret_key3"
#define data_mem_size 200
#define k_mem_size 4096
#define int_size 4
#define node_size sizeof(proc_inf)
#define MAX_PAYLOAD 1024
#define MAX_MSGSIZE 1024
#define NETLINK_TEST 20

#endif