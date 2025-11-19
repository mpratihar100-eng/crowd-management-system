cmake_minimum_required(VERSION 3.20)
project(CrowdManagementSystem C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_subdirectory(services/cv_worker)
add_subdirectory(services/api)
