cmake_minimum_required(VERSION 3.16)
project(RTKLIB C)

set(CMAKE_C_STANDARD 99)

include_directories("src")
link_libraries(m pthread)
aux_source_directory("src" rtklib_srcs)
aux_source_directory("src/rcv" rtklib_rcv_srcs)

add_executable(RTKLIB main.c ${rtklib_srcs} ${rtklib_rcv_srcs})
