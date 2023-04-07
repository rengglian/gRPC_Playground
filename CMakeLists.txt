cmake_minimum_required(VERSION 3.20)


project("RPC_Example" VERSION 1.0.0 DESCRIPTION "gRPC Test" LANGUAGES C CXX)

# Global CMake variables are set here
set(CMAKE_CXX_STANDARD              17)
#set(CMAKE_CXX_STANDARD_REQUIRED     ON)
#set(CMAKE_CXX_EXTENSIONS            OFF)
#set(CMAKE_EXPORT_COMPILE_COMMANDS   ON)

message(STATUS "Main CMakeList: ${PROJECT_NAME}")

add_subdirectory(proto)
add_subdirectory(src)