cmake_minimum_required(VERSION 3.20)
project(snowflake CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(FLTK REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(snowflake
  main.cpp
  Graph_lib/Graph.cpp
  Graph_lib/GUI.cpp
  Graph_lib/Window.cpp
  Graph_lib/Simple_window.cpp
)

target_include_directories(snowflake PRIVATE
  ${FLTK_INCLUDE_DIR}
  ${CMAKE_SOURCE_DIR}
)

target_link_libraries(snowflake PRIVATE
  ${FLTK_LIBRARIES}
  ${OPENGL_LIBRARIES}
)
