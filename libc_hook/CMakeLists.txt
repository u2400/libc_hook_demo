cmake_minimum_required(VERSION 3.13)
project(libc_hook C)

set(CMAKE_C_STANDARD 99)

add_compile_options(-lrt)
message(STATUS "optional:-lrt")

add_executable(libc_hook main.c)