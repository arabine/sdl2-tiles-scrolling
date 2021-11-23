cmake_minimum_required(VERSION 3.5)

set(PROJ_NAME sdl2-tiles-scrolling )

project(${PROJ_NAME} LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2)

add_executable(${PROJ_NAME} main.cpp glad.c)
target_include_directories(${PROJ_NAME} PUBLIC glad)
target_link_libraries(${PROJ_NAME} SDL2 SDL2_image)
