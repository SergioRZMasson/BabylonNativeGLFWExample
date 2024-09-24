cmake_minimum_required(VERSION 3.20.0)
project(BabylonNativeExample VERSION 0.1.0)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_STANDARD} /W3 /Zc:__cplusplus /Zc:preprocessor")
endif()

add_subdirectory(Dependencies)
add_subdirectory(Examples)
