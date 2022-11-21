# Made by Luca de Raad

cmake_minimum_required(VERSION 3.0.0)

project(HeadposeEstimation)

add_executable(Pinocchio network_test.cpp)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

target_link_libraries(Pinocchio Threads::Threads)
target_precompile_headers(Pinocchio PRIVATE httplib.h)
#add_dependencies(Pinocchio httplibpch)

# Include httplib pre-compiled header
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -include-pch ${CMAKE_CURRENT_BINARY_DIR}/httplib.h.pch")

# How to generate httplib pre-compiled header
#add_custom_target(httplibpch COMMAND clang -x c++-header ${CMAKE_CURRENT_SOURCE_DIR}/httplib.h -o ${CMAKE_CURRENT_BINARY_DIR}/httplib.h.pch)

