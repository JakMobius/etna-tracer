cmake_minimum_required(VERSION 3.10)
project(etna-tracer)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

include(FindPkgConfig)
find_package(etna REQUIRED)
find_package(Vulkan REQUIRED)

set(CMAKE_CXX_COMPILER clang++)

set(CMAKE_CXX_STANDARD 20)

set(WARNING_FLAGS "-Wall -Wextra -Wno-multichar -Wno-unused-result -Wno-deprecated-declarations")

set(CMAKE_EXE_LINKER_FLAGS "-pthread")
set(CMAKE_CXX_FLAGS   "-g ${WARNING_FLAGS} -fdeclspec -fsanitize=address")

file(GLOB_RECURSE files "${CMAKE_SOURCE_DIR}/src/*.cpp" "${CMAKE_SOURCE_DIR}/src/*.hpp")
add_executable(etna-tracer ${files})

include_directories(etna-tracer ${ETNA_INCLUDE_DIR} ${Vulkan_INCLUDE_DIR})
target_link_libraries(etna-tracer ${ETNA_LIBRARY})