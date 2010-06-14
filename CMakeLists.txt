project(halter)

cmake_minimum_required(VERSION 2.6)

include_directories(${halter_SOURCE_DIR}/pvm3/include)
link_directories(${halter_SOURCE_DIR}/pvm3/lib ${halter_SOURCE_DIR}/pvm3/lib/LINUX64)
link_libraries(pvm3)

add_executable(halter tids.cpp joblist.cpp app.cpp vclock.cpp monitor.cpp master.cpp)

