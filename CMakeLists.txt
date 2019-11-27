cmake_minimum_required(VERSION 3.13)
project(libc_preload C)

set(CMAKE_C_STANDARD 99)

add_executable(libc_preload main.c get_proc_inf.c get_proc_inf.h)