cmake_minimum_required(VERSION 3.13)

project(engines_ai)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SOURCES1 . ./*.[ch]pp)
file(GLOB_RECURSE SOURCES2 . ./*.[ch])

add_executable(engines_ai ${SOURCES1} ${SOURCES2})
target_link_libraries(engines_ai PUBLIC project_options project_warnings)
target_link_libraries(engines_ai PUBLIC raylib)

