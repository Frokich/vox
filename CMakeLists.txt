cmake_minimum_required(VERSION 3.14)
project(vox VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find Lua
find_package(Lua 5.4 REQUIRED)

# Find ncurses
find_package(Curses REQUIRED)

# Sources
set(SOURCES
    src/main.cpp
    src/editor.cpp
    src/terminal.cpp
    src/row_operations.cpp
    src/file_io.cpp
    src/input.cpp
    src/lua_integration.cpp
)

# Executable
add_executable(vox ${SOURCES})

# Includes
target_include_directories(vox PRIVATE include)

# Libraries
target_link_libraries(vox PRIVATE ${LUA_LIBRARIES} ${CURSES_LIBRARIES})
target_include_directories(vox PRIVATE ${LUA_INCLUDE_DIR} ${CURSES_INCLUDE_DIRS})

# Compiler flags
target_compile_options(vox PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
)

# Install
install(TARGETS vox DESTINATION bin)
install(DIRECTORY scripts/ DESTINATION share/vox/scripts)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Lua ${LUA_VERSION_STRING} found: ${LUA_LIBRARIES}")
message(STATUS "ncurses found: ${CURSES_LIBRARIES}")
