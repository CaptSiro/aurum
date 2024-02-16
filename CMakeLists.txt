cmake_minimum_required(VERSION 3.26)
project(aurum C)

set(CMAKE_C_STANDARD 11)

add_executable(aurum
        main.c
        array.h
        queue.h
        types.h
        types.c
        tokenizer.c
        tokenizer.h
        bytecode.c
        bytecode.h
        memory.c
        memory.h
        vm.c
        vm.h
        vm.c
        vm.h
        map.h
        ast.c
        ast.h
        ast.h)
