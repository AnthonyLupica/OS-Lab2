# @file CMakeLists.txt
# 
# CMake files for OS - Lab 2  (pthreads and semaphores)

# Can be run by any version of CMake >= this
cmake_minimum_required(VERSION 3.10.2)

# project info
project(smoker C)

# Source files 
set(SOURCE smoker.c)

# convert application
add_executable(smoker ${SOURCE})

# Turn on warnings
if (MSVC)
    # warning level 4
    add_compile_options(/W4)
else()
    # standard warnings
    add_compile_options(-Wall)
endif()

# run command
add_custom_target(run
        COMMENT "Run"
        COMMAND $<TARGET_FILE:smoker>
        DEPENDS smoker
        USES_TERMINAL
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# link in pthreads
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(smoker PRIVATE Threads::Threads)