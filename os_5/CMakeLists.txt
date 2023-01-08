cmake_minimum_required(VERSION 3.22)
project(lab5 C)

set(CMAKE_C_STANDARD 99)

add_library(mylib1 STATIC lib1.c lib.h)
add_library(mylib2 STATIC lib2.c lib.h)
add_executable(main1 main1.c)
target_link_libraries(main1 mylib1 mylib2 m)

add_library(mylib1x SHARED lib1.c lib.h)
add_library(mylib2x SHARED lib2.c lib.h)
add_executable(main2 main2.c)
target_link_libraries(mylib1x m)
target_link_libraries(mylib2x m)
target_link_libraries(main2 ${CMAKE_DL_LIBS})