cmake_minimum_required(VERSION 3.10)
set(TARGET polycur)
project(${TARGET})

set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)
add_subdirectory(${CMAKE_SOURCE_DIR}/third_party/glfw)

include_directories(include)
include_directories(${CMAKE_SOURCE_DIR}/third_party/glfw/include)
set(SOURCES src/glad.c src/main.cpp src/shader.cpp src/lodepng.cpp)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS -O2)

configure_file(resources/vert ${CMAKE_BINARY_DIR}/vert COPYONLY)
configure_file(resources/frag ${CMAKE_BINARY_DIR}/frag COPYONLY)

add_executable(${TARGET} ${SOURCES})
target_link_libraries(${TARGET} glfw ${GLFW_LIBRARIES})

if (UNIX)
	#target_link_libraries(${TARGET} )
endif (UNIX)

#target_link_libraries(${TARGET} GL GLU glfw3 X11 Xxf86vm Xrandr pthread Xi dl Xinerama Xcursor)



