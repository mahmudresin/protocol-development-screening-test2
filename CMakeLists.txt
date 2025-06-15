cmake_minimum_required(VERSION 3.15)
project(DistributedTaskExecution)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler-specific options
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pthread")
endif()

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
endif()

# Find required packages
find_package(Threads REQUIRED)

# Create the executable
add_executable(distributed_app main.cpp)

# Link threads library
target_link_libraries(distributed_app Threads::Threads)

# Optional: Add installation rules
install(TARGETS distributed_app DESTINATION bin)