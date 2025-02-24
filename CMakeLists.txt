
find_package(Eigen3 REQUIRED NO_MODULE)
cmake_minimum_required(VERSION 3.5.0)
set(CMAKE_CXX_STANDARD 17)

project(KalmanFilter VERSION 0.1.0 LANGUAGES C CXX)

add_executable(Tracker ArrayController.cpp AssignmentOptimal.cpp Filter.cpp Source.cpp Track_GNN.cpp Tracker_GNN.cpp)

find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})
