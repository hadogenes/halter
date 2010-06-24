project(halter)

cmake_minimum_required(VERSION 2.6)

include_directories(${halter_SOURCE_DIR}/pvm3/include)
link_directories(${halter_SOURCE_DIR}/pvm3/lib ${halter_SOURCE_DIR}/pvm3/lib/LINUX64)
link_libraries(pvm3 gpvm3)

add_executable(starter joblist.cpp init.cpp tids.cpp vclock.cpp starter.cpp)
add_executable(slave joblist.cpp tids.cpp app.cpp vclock.cpp monitor.cpp slave.cpp)
add_executable(halter joblist.cpp tids.cpp halter.cpp)

