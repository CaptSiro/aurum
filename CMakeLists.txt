cmake_minimum_required(VERSION 3.26)
project(aurum C)

set(CMAKE_C_STANDARD 11)

add_executable(aurum
        main.c
        queue.h
        tokenizer.c
        types.h
        array.h
        null-guards.h
        types.c
        tokenizer.c
        tokenizer.h)
