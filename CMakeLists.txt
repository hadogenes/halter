project(halter)

cmake_minimum_required(VERSION 2.6)

include_directories(${halter_SOURCE_DIR}/pvm3/include ${halter_SOURCE_DIR}/include)
link_directories(${halter_SOURCE_DIR}/pvm3/lib ${halter_SOURCE_DIR}/pvm3/lib/LINUX64 ${halter_SOURCE_DIR}/lib)
link_libraries(pvm3 gpvm3)

add_executable(starter joblist.cpp init.cpp tids.cpp starter.cpp)
add_executable(slave joblist.cpp tids.cpp app.cpp monitor.cpp slave.cpp)
add_executable(halter joblist.cpp tids.cpp halter.cpp)
add_executable(resumer joblist.cpp init.cpp tids.cpp resumer.cpp)

target_link_libraries(halter ticpp)
target_link_libraries(resumer ticpp)