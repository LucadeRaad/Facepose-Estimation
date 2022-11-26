# Made by Luca de Raad

cmake_minimum_required(VERSION 3.0.0)

project(HeadposeEstimation)

add_executable(Pinocchio webcam_head_pose.cpp)
add_executable(Maia network_test.cpp)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
# See if this solves the speed problem
set(CMAKE_EXE_LINKER_FLAGS "-static")

find_package(Threads REQUIRED)
target_link_libraries(Pinocchio Threads::Threads)
target_precompile_headers(Pinocchio PRIVATE httplib.h)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS} /usr/local/include/opencv4)
target_link_libraries(Pinocchio opencv_core opencv_imgproc opencv_video ${EXTERNAL_LIBS})
target_link_libraries(Pinocchio ${OpenCV_LIBS})
target_link_libraries(Pinocchio dlib lapack blas)
add_compile_options(Pinocchio -O3)

target_link_libraries(Maia Threads::Threads)
target_precompile_headers(Maia PRIVATE httplib.h)

