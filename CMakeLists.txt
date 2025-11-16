cmake_minimum_required(VERSION 3.15)

project(linecheck)
set(CMAKE_CXX_STANDARD 20)


add_executable(linecheck
    src/main.cpp 
)

# ./include/
# target_include_directories(line_checker
#     PRIVATE
#         ${CMAKE_CURRENT_SOURCE_DIR}/include
# )

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
