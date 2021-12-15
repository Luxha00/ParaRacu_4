cmake_minimum_required(VERSION 3.20)
project(ParaRacu_4)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-Wall -pedantic -Wextra -fopenmp")


add_executable(ParaRacu_4 main.cpp)
