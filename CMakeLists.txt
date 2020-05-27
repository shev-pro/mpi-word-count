cmake_minimum_required(VERSION 3.10)
project(mpi_word_count C)

set(CMAKE_C_STANDARD 99)

add_executable(mpi_word_count main.c fs_utils.c fs_utils.h hash_map.h linked_list.h errors.h wc_constants.h log.c log.h wc_utils.c wc_utils.h hash_map_crc.h)

# Link against MPI
target_link_libraries(mpi_word_count ${MPI_LIBRARIES})

# Include MPI includes
include_directories(${MPI_INCLUDE_PATH})
SET(CMAKE_C_COMPILER mpicc)
SET(CMAKE_CXX_COMPILER mpicxx)
if(MPI_COMPILE_FLAGS)
    set_target_properties(mpi_word_count PROPERTIES
            COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()
if(MPI_LINK_FLAGS)
    set_target_properties(mpi_word_count PROPERTIES
            LINK_FLAGS "${MPI_LINK_FLAGS}")
endif()