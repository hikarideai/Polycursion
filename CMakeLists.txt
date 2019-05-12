cmake_minimum_required(VERSION 3.10)
set(TARGET polycur)
project(${TARGET})

include_directories(include)
set(SOURCES src/glad.c src/main.cpp src/shader.cpp src/lodepng.cpp)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS -O2)

add_executable(${TARGET} ${SOURCES})

target_link_libraries(${TARGET} GL GLU glfw3 X11 Xxf86vm Xrandr pthread Xi dl Xinerama Xcursor)



