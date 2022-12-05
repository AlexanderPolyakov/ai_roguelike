cmake_minimum_required(VERSION 3.13)

project(w7)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE HW7_SOURCES1 . ./*.[ch]pp)
file(GLOB_RECURSE HW7_SOURCES2 . ./*.[ch])

add_executable(hw7 ${HW7_SOURCES1} ${HW7_SOURCES2})
target_link_libraries(hw7 PUBLIC project_options project_warnings)
target_link_libraries(hw7 PUBLIC raylib flecs)

