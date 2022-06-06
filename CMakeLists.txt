cmake_minimum_required(VERSION 3.16)
project(rtklib C)
project(grizz CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

include_directories("src" "3part")

link_libraries(m pthread)

aux_source_directory("src" rtklib_srcs)
aux_source_directory("src/rcv" rtklib_rcv_srcs)

add_executable(rtklib main.c ${rtklib_srcs} ${rtklib_rcv_srcs})

aux_source_directory("grizz" grizz_srcs)
add_executable(grizz grizz.cpp ${rtklib_srcs} ${rtklib_rcv_srcs} ${grizz_srcs})
