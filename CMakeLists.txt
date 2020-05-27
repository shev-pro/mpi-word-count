cmake_minimum_required(VERSION 3.10)
project(mpi_word_count C)

set(CMAKE_C_STANDARD 99)

add_executable(mpi_word_count main.c fs_utils.c fs_utils.h hash_map.h linked_list.h errors.h wc_constants.h log.c log.h wc_utils.c wc_utils.h hash_map_crc.h)