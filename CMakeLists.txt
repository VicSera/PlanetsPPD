cmake_minimum_required(VERSION 3.21)
project(Planets)

set(CMAKE_CXX_STANDARD 23)
add_compile_options("-pthread")
add_link_options("-static-libgcc" "-static-libstdc++")

find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})

add_executable(Planets main.cpp physics.cpp graphics.cpp)
add_executable(Planets-threads main-threads.cpp physics.cpp graphics.cpp ThreadPool.cpp ThreadPool.h)
add_executable(Planets-mpi main-mpi.cpp physics.cpp graphics.cpp)


target_link_libraries(Planets-mpi PUBLIC ${MPI_LIBRARIES})

if(MPI_COMPILE_FLAGS)
    set_target_properties(Planets-mpi PROPERTIES COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()

if(MPI_LINK_FLAGS)
    set_target_properties(Planets-mpi PROPERTIES LINK_FLAGS "${MPI_LINK_FLAGS}")
endif()