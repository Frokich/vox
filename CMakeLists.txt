cmake_minimum_required(VERSION 3.14)
project(vox VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    sol2
    GIT_REPOSITORY https://github.com/ThePhD/sol2.git
    GIT_TAG v3.3.0
)

FetchContent_GetProperties(sol2)
if(NOT sol2_POPULATED)
    FetchContent_Populate(sol2)
    # Добавляем путь к заголовкам sol2 в include-директории проекта
    include_directories(${sol2_SOURCE_DIR}/include)
endif()

find_package(Lua 5.4 REQUIRED)

add_executable(vox
    src/main.cpp
    src/Editor.cpp
    src/LuaManager.cpp
    src/Terminal.cpp
)

target_include_directories(vox PRIVATE src ${LUA_INCLUDE_DIR})

target_link_libraries(vox PRIVATE ${LUA_LIBRARIES})

target_compile_definitions(vox PRIVATE SOL_ALL_SAFETIES_ON=1)
