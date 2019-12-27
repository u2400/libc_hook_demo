//
// Created by user on 12/17/19.
//

#ifndef UTIL_H
#define UTIL_H
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

long unsigned int jiffies_to_unix_time(long unsigned int jiffies);
long get_file_length(FILE* f);
int convertDecimalToOctal(int decimalNumber);
char *format_path(int Pid, char *path);
char *read_file(char *path);
void *new_share_mem(char *name, size_t size);
void *open_share_mem(char *name, size_t size);

#endif