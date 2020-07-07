cmake_minimum_required(VERSION 3.10)
project(mpi_word_count C)

set(CMAKE_C_STANDARD 99)

add_executable(mpi_word_count src/main.c src/fs_utils.c src/fs_utils.h src/hash_map.h src/linked_list.h src/errors.h src/wc_constants.h src/log.c src/log.h src/wc_utils.c src/wc_utils.h src/hash_map_crc.h src/wc_mpi_helpers.h src/wc_core.h)

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