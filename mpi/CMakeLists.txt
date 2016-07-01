cmake_minimum_required(VERSION 2.8)

project(msu_supercomputing_academy)

set(CMAKE_C_COMPILER mpicc)
set(CMAKE_CXX_COMPILER mpicxx)
ADD_DEFINITIONS( "-DUSE_MPI" )

add_executable(task1.exe src/task1.c)
add_executable(task2.exe src/task2.c)
add_executable(task3.exe src/task3.cpp)
add_executable(task4.exe src/task4.cpp)
add_executable(task5.exe src/task5.cpp)

