cmake_minimum_required (VERSION 2.6)
project (Tutorial)


#INCLUDE_DIRECTORIES(/usr/include/)
#LINK_DIRECTORIES(your/library/dir)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
add_executable(Tutorial simple_test.cpp)
TARGET_LINK_LIBRARIES(Tutorial fann)
