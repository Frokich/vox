cmake_minimum_required(VERSION 3.10)
project(VoxEditor)

set(CMAKE_CXX_STANDARD 17)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

find_package(PkgConfig REQUIRED)
pkg_check_modules(LUA REQUIRED lua5.4)
include_directories(${LUA_INCLUDE_DIRS})
link_directories(${LUA_LIBRARY_DIRS})

add_executable(vox src/editor.cpp)
target_link_libraries(vox ${CURSES_LIBRARIES} ${LUA_LIBRARIES})

