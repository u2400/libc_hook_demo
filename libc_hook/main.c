/*
 * @Date: 2019-12-26 20:31:14
 * @LastEditors  : u2400
 * @LastEditTime : 2020-01-01 21:53:16
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include "mem_share.h"
#include <unistd.h>
#include <dlfcn.h>

typedef ssize_t (*execve_func_t)(const char* filename, char* const argv[], char* const envp[]);
static execve_func_t old_execve = NULL;
int execve(const char* filename, char* const argv[], char* const envp[]) {
    mem_share_init();
    int Pid = getpid();
    push_data(Pid);
    old_execve = dlsym(RTLD_NEXT, "execve");
    return old_execve(filename, argv, envp);
}